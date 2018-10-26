#pragma once

#pragma once

#include <iostream>
#pragma once

#include <any>
// #include <iostream>
#include <typeinfo>
#include <functional>
#include <memory>

#pragma once

#include <any>
// #include <iostream>
#include <typeinfo>
#include <functional>
#include <memory>

namespace pythonic
{
	class elem_value;

	class iterator
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef elem_value value_type;
		typedef size_t difference_type;
		typedef elem_value * pointer;
		typedef elem_value & reference;

		typedef std::shared_ptr<void> ContextPtr;
		typedef std::function<void(ContextPtr)> Increment;
		typedef std::function<bool(ContextPtr, const iterator &)> NotEqual;
		typedef std::function<elem_value &(ContextPtr)> Visit;
	protected:
		ContextPtr context;
		const Increment incre;
		const NotEqual not_equal;
		const Visit visit;
	public:
		iterator(Increment incre, NotEqual not_equal, Visit visit, const ContextPtr & context) :
			incre(incre), not_equal(not_equal), visit(visit), context(context)
		{}

		void operator++ ()
		{
			incre(context);
		}

		elem_value & operator* ()
		{
			return visit(context);
		}

		bool operator != (const iterator & that) const noexcept
		{
			return not_equal(context, that);
		}

		bool operator == (const iterator & that) const noexcept
		{
			return !not_equal(context, that);
		}

		const ContextPtr & get_context() const noexcept
		{
			return context;
		}

		template<typename Type>
		static Type & visitAs(const ContextPtr & ptr)
		{
			return *reinterpret_cast<Type*>(ptr.get());
		}

		template<typename Type>
		static const Type & visitAsConst(const ContextPtr & ptr)
		{
			return *reinterpret_cast<Type*>(ptr.get());
		}

		template<typename Iter>
		static inline iterator from_iter(const Iter & iter)
		{
			Iter * p_iter = new Iter(iter);
			ContextPtr ptr(p_iter);

			return iterator(
				[](ContextPtr ptr) -> void { visitAs<Iter>(ptr)++; },
				[](ContextPtr ptr, const iterator & that) -> bool { return visitAs<Iter>(ptr) != visitAs<Iter>(that.get_context()); },
				[](ContextPtr ptr) -> elem_value & { return *visitAs<Iter>(ptr); },
				ptr
			);
		}
	};

	class const_iterator : public iterator
	{
	protected:
		typedef std::function<const elem_value &(ContextPtr)> ConstVisit;
	private:
		const ConstVisit const_visit;
	public:
		const_iterator(Increment incre, NotEqual not_equal, ConstVisit visit, const ContextPtr & context) :
			iterator(incre, not_equal, nullptr, context), const_visit(visit)
		{
		}

		const elem_value & operator* () const
		{
			return const_visit(context);
		}

		template<typename Iter>
		static inline const_iterator from_const_iter(const Iter & iter)
		{
			Iter * p_iter = new Iter(iter);
			ContextPtr ptr(p_iter);
			return const_iterator(
				[](ContextPtr ptr) -> void { visitAs<Iter>(ptr)++; },
				[](ContextPtr ptr, const iterator & that) -> bool { return visitAs<Iter>(ptr) != visitAs<Iter>(that.get_context()); },
				[](ContextPtr ptr) -> const elem_value & { return *visitAsConst<Iter>(ptr); },
				ptr
			);
		}
	};
}

namespace pythonic
{
	class elem_value;
	class str;

	class container
	{
	protected:
	public:
		typedef elem_value elem_t;

		virtual bool __equal__(const container & that) const noexcept = 0;
		virtual size_t __len__() const noexcept = 0;
		virtual str __str__() const noexcept = 0;

		virtual const_iterator begin() const noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		bool operator == (const container & container) const
		{
			return __equal__(container);
		}
	};

	typedef container::elem_t any;

}

namespace pythonic
{
	class range
	{
	private:
		int32_t begin_v = 0;
		int32_t end_v;
		int32_t step = 1;
	public:
		range(int32_t end)
			: end_v(end)
		{}

		range(int32_t start, int32_t end)
			: begin_v(start), end_v(end)
		{}

		range(int32_t start, int32_t end, int32_t step)
			: begin_v(start), step(step)
		{
			if (step > 0)
				end_v = (end - start + step - 1) / step * step + start;
			if (step < 0)
				end_v = start - (start - end + ((-1) * step - 1)) / ((-1) * step) * ((-1) * step);
		}

		class iterator
		{
			int32_t value;
			int32_t step;
		public:
			iterator(const int32_t v, const int32_t step) : value(v), step(step) {}

			int32_t operator *()
			{
				return value;
			}

			void operator ++()
			{
				value += step;
			}
			/* Only for end judgement */
			bool operator !=(const iterator & another)
			{
				return value != another.value;
			}
		};

		iterator begin()
		{
			return iterator(begin_v, step);
		}

		iterator end()
		{
			return iterator(end_v, step);
		}
	};
}
#pragma once

#include <functional>
#include <tuple>
#include <utility>
#include <type_traits>



namespace pythonic
{

	template<typename T>
	struct function_traits;

	template<typename R, typename ...Args>
	struct function_traits<std::function<R(Args...)>>
	{
		static const size_t nargs = sizeof...(Args);

		typedef R result_type;

		template<size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		};
	};

	template<typename Ret>
	using ret_or_dummy = std::conditional_t<std::is_void<Ret>::value, size_t, Ret>;

	template<typename FuncType>
	struct match_impl_base;

	template<typename Ret, typename First>
	struct match_impl_base<std::function<Ret(First)>>
	{
		typedef container::elem_t elem_t;

		typedef ret_or_dummy<Ret> NonVoidRet;

		template<bool isVoid>
		static inline NonVoidRet match_first
		(const elem_t & elem, std::function<Ret(First)> && first, bool & success);

		template<>
		static inline NonVoidRet match_first<false>
			(const elem_t & elem, std::function<Ret(First)> && first, bool & success)
		{
			if (elem.isinstance<First>())
			{
				success = true;
				return first(elem.as<First>());
			}
			else
			{
				return NonVoidRet();
			}
		}

		template<>
		static inline NonVoidRet match_first<true>
			(const elem_t & elem, std::function<Ret(First)> && first, bool & success)
		{

			if (elem.isinstance<First>())
			{
				success = true;
				first(elem.as<First>());
			}
			return NonVoidRet();
		}
	};

	template<typename Ret, typename ...FuncTypes>
	struct match_impl;

	template<typename Ret, typename First>
	struct match_impl<std::function<Ret(First)>> : public match_impl_base<std::function<Ret(First)>>
	{
		typedef match_impl_base<std::function<Ret(First)>> _Base;
		typedef typename _Base::NonVoidRet NonVoidRet;

		static NonVoidRet match(const _Base::elem_t & elem, std::function<Ret(First)> && function)
		{
			bool success = false;
			
			NonVoidRet result = _Base::match_first<std::is_void_v<Ret>>(
				std::forward<const typename _Base::elem_t&>(elem),
				std::forward<std::function<Ret(First)>>(function), 
				success);
			if (success)
			{
				return result;
			}
			else
			{
				return _Base::NonVoidRet();
			}
		}
	};

	template<typename Ret, 
		typename First, typename ...Rest>
	struct match_impl<std::function<Ret(First)>, Rest...> : public match_impl_base<std::function<Ret(First)>>
	{
		typedef match_impl_base<std::function<Ret(First)>> _Base;
		typedef typename _Base::NonVoidRet NonVoidRet;

		static NonVoidRet match(const _Base::elem_t & elem, std::function<Ret(First)> && function, Rest &&... rest)
		{
			bool success = false;
			NonVoidRet result = _Base::match_first<std::is_void_v<Ret>>(
				std::forward<const typename _Base::elem_t&>(elem),
				std::forward<std::function<Ret(First)>>(function), 
				success);
			if (success)
			{
				return result;
			}
			else
			{
				return match_impl<Rest...>::match(std::forward<const typename _Base::elem_t&>(elem), std::forward<Rest>(rest)...);
			}
		}
	};

	template<typename ...Types>
	struct first
	{
		typedef std::tuple_element_t<0, std::tuple<Types...>> value;
	};
	
	template<typename ...Funcs>
	static
	auto match(const container::elem_t & elem, Funcs &&... funcs)
	{
		return match(std::forward<const container::elem_t&>(elem), std::function(funcs)...);
	}
	
	template<typename R, typename ...ArgTypes, typename NonVoidRet = ret_or_dummy<R>>
	static
	NonVoidRet
	match(const container::elem_t & elem, std::function<R(ArgTypes)> &&... funcs)
	{
		return match_impl<std::function<R(ArgTypes)>...>::match(
			std::forward<const container::elem_t&>(elem),
			std::forward<std::function<R(ArgTypes)>>(funcs)...
		);
	}
}


#pragma once

#include <iostream>

#include <any>
#include <vector>
#include <initializer_list>

#include <cassert>


#pragma once
#include <any>
#include <iostream>
#include <typeinfo>
#include <string>

#pragma once

#include <string>

#define BASIC_TYPES_LIST(B)	\
	B(int8_t), 			\
	B(int16_t),			\
	B(int32_t),			\
	B(int64_t),			\
	B(uint16_t),		\
	B(uint32_t),		\
	B(uint32_t),		\
	B(uint64_t),		\
	B(char), 			\
	B(char16_t),		\
	B(char32_t),		\
	B(wchar_t),			\
	B(char*), 			\
	B(const char*), 	\
	B(std::string),		\





namespace pythonic
{
	class list;

	class elem_value
	{
		std::any value;
		std::shared_ptr<container> cont;
	public:
		template<class ...Args>
		elem_value(Args... args) :
			value(std::forward<Args>(args)...)
		{
		}

		elem_value(const list & l)
		{
			// std::cout << "use elem_value list & l" << std::endl;
			cont = std::dynamic_pointer_cast<container>(std::make_shared<list>(l));
		}
		
		elem_value(list * c)
		{
			cont = std::shared_ptr<container>(reinterpret_cast<container*>(c));
		}

		template<typename T>
		inline T as() const
		{
			return std::any_cast<T>(this->value);
		}

		template<>
		inline any as<any>() const
		{
			return *this;
		}

		template<>
		inline const any & as<const any &>() const
		{
			return *this;
		}

		template<typename T>
		inline T asMut()
		{
			return std::any_cast<T>(this->value);
		}

		template<>
		inline any asMut<any>()
		{
			return *this;
		}

		template<>
		inline const any & asMut<const any &>()
		{
			return *this;
		}
		
		static
		inline any remove_const(const elem_value & v)
		{
			elem_value v1 = v;
			return v1;
		}

		template<typename T>
		inline bool isinstance() const
		{
			return value.type() == typeid(T);
		}

		template<>
		inline bool isinstance<any>() const
		{
			return true;
		}

		template<>
		inline bool isinstance<const any &>() const
		{
			// std::cout << value.type().name() << ". " << std::endl;
			return true;
		}

		std::any & get_value()
		{
			return value;
		}

		auto & get_cont() { return cont; }

		const auto & get_cont() const { return cont; }

#define COMPARE(type) [=](type t) { \
			return as<type>() == that.as<type>(); \
		}

		bool operator == (const elem_value & that) const noexcept
		{
			return match(
				that,
				BASIC_TYPES_LIST(COMPARE)

				[=](const any & t)
				{
					return *cont == *(t.cont);
				}
			);
		}
	};

#undef COMPARE
}

#pragma once

#include <string>






namespace std
{
	inline
	std::string to_string(char * s)
	{
		return std::string(s);
	}

	inline
	std::string to_string(const char * s)
	{
		return std::string(s);
	}

	inline
	std::string to_string(const std::string & s)
	{
		return std::string(s);
	}

}

namespace pythonic
{
	class str : public container
	{
	private:
		mutable any cur_char;
		std::string content;
	public:

		explicit str(any c)
			: content(to_string(c))
		{
		}

		str()
		{
		}

		template<typename Iterable>
		str join(const Iterable & iter)
		{
			// std::cerr << "Use join" << std::endl;
			std::string s;
			for (const auto & x : iter)
			{
				s += str(x).get_content();
				s += get_content();
			}
			return str(s.substr(0, s.length() - len(*this)));
		}

		/* Implements __equal__ */
		virtual bool __equal__(const container & container) const noexcept override
		{
			if (dynamic_cast<const str*>(&container) == nullptr)
			{
				return false;
			}
			else
			{
				return __len__() == container.__len__() && std::equal(begin(), end(), container.begin());
			}
		}

		/* Implements __len__ */
		virtual size_t __len__() const noexcept override
		{
			return content.size();
		}

		/* Implements __str__ */
		virtual str __str__() const noexcept override
		{
			return *this;
		}

		str operator + (const str & b)
		{
			return str(content + b.content);
		}

		auto & get_content() { return content; }

		const auto & get_content() const { return content; }

		virtual const_iterator begin() const noexcept
		{
			return get_iter(content.begin());
		}

		virtual const_iterator end() const noexcept
		{
			return get_iter(content.end());
		}

	private:
		const_iterator get_iter(const std::string::const_iterator & tg) const
		{
			using ContextPtr = const_iterator::ContextPtr;
			using Iter = std::string::const_iterator;

			Iter * iter = new Iter(tg);
			ContextPtr ptr(iter);

			return const_iterator(
				[](ContextPtr ptr) -> void { iterator::visitAs<Iter>(ptr)++; },
				[](ContextPtr ptr, const iterator & that) -> bool { return iterator::visitAs<Iter>(ptr) != iterator::visitAs<Iter>(that.get_context()); },
				[=](ContextPtr ptr) -> const elem_value & { cur_char = any(*iterator::visitAsConst<Iter>(ptr)); return cur_char; },
				ptr
			);
		}

#define TO_STRING(type) [](type a){ return std::to_string(a); }

		_NODISCARD static inline std::string to_string(const pythonic::any & any)
		{
			return match(
				any,
				BASIC_TYPES_LIST(TO_STRING)
				[=](const pythonic::any & t) {
				return t.get_cont()->__str__().get_content();
			});
		}
	};

}

namespace std
{

	inline
	ostream& operator<<(ostream& os, const pythonic::str& s)
	{
		os << s.get_content();
		return os;
	}
}

namespace pythonic 
{ 
	struct init_elem;\

	class list : public container
	{
	public:
		typedef std::vector<elem_t> container_t;
		// static int id;
	private:
		// int self_id;
		container_t content;
	public:

		inline explicit list(const std::initializer_list<init_elem> & il);

		list(const list & l)
		{
			//self_id = l.self_id + 1000;
			content = l.content;
			/*
			std::cerr << "list id: " << self_id << std::endl;
			std::cerr << "source id: " << l.self_id << std::endl;
			std::cerr << "list copied, len: " << this->__len__() << std::endl;
			*/
			//if (self_id > 1010)
			//	assert(this->__len__() > 0);
		}

		list(list && l)
		{
			//self_id = l.self_id + 1000;
			content = std::move(l.content);
			//std::cerr << "list id: " << self_id << std::endl;
			//std::cerr << "source id: " << l.self_id << std::endl;
			//std::cerr << "list moved, len: " << this->__len__() << std::endl;
			//if (self_id > 1010)
			//	assert(this->__len__() > 0);
		}

		list & operator = (list & l)
		{
			//self_id = l.self_id;
			content = l.content;
		}

		list & operator = (list && l)
		{
			//self_id = l.self_id;
			content = std::move(l.content);
		}

		explicit list(const container_t & c)
		{
			content = c;
		}

		explicit list(const container_t && c)
		{
			content = c;
		}

		list()
		{
		}

		~list()
		{
			//std::cerr << "list id: " << self_id << std::endl;
			//std::cerr << "list destroyed, len: " << this->__len__() << std::endl;
		}

		/* Implements __getitem__ */ // TODO: virtualize this func
		elem_t & operator [](const elem_t & index)
		{
			if (index.isinstance<int>())
			{
				return content.at(index.as<int>());
			}
			else if (index.isinstance<size_t>())
			{
				return content.at(index.as<size_t>());
			}
			else
			{
				throw std::exception("list index incorrect: int or size_t allowed only");
			}
		}

		/* Slicing */
		list operator () (size_t a, size_t b)
		{
			return list(container_t(content.begin() + a, content.begin() + b));
		}

		list & append(const elem_t & elem)
		{
			content.push_back(elem);
			return *this;
		}

		template<typename Iterable>
		list & extend(const Iterable & container)
		{
			if ((size_t)(&container) != (size_t)(this))
			{
				for (const auto & x : container)
				{
					append(x);
				}
			}
			else
			{
				throw std::exception("list cannot extend from itself. ");
			}
			return *this;
		}

		list & extend(std::initializer_list<elem_t> c)
		{
			for (const auto & x : c)
			{
				append(x);
			}
			return *this;
		}

		list & insert(size_t place, const elem_t & elem)
		{
			content.insert(content.begin() + place, elem);
			return *this;
		}

		list & remove(const elem_t & index)
		{
			for (auto iter = content.begin(); iter != content.end(); iter++)
			{
				if (*iter == index)
				{
					content.erase(iter);
				}
			}
			return *this;
		}

		/* Implements __str__ */
		virtual str __str__() const noexcept override
		{
			//std::cerr << "list id: " << self_id << std::endl;
			//std::cerr << "__str__(): this len: " << this->__len__() << std::endl;
			return str("[") + str(", ").join(*this) + str("]");
		}

		/* Implements __equal__ */
		virtual bool __equal__(const container & container) const noexcept override
		{
			if (dynamic_cast<const list*>(&container) == nullptr)
			{
				return false;
			}
			else
			{
				return __len__() == container.__len__() && std::equal(begin(), end(), container.begin());
			}
		}

		/* Implements __len__ */
		virtual size_t __len__() const noexcept override
		{
			return content.size();
		}

		virtual iterator begin() noexcept
		{
			return iterator::from_iter(content.begin());
		}

		virtual const_iterator begin() const noexcept
		{
			return const_iterator::from_const_iter(content.begin());
		}

		virtual iterator end() noexcept
		{
			return iterator::from_iter(content.end());
		}

		virtual const_iterator end() const noexcept
		{
			return const_iterator::from_const_iter(content.end());
		}
	};

	struct init_elem
	{
		enum Type
		{
			Elem = 1,
			List = 2
		} type;
		elem_value elem;
		list l;

		template<typename Any, class dummy =
			std::enable_if<std::is_same_v<std::remove_cv_t<
			std::remove_reference_t<Any>>,
			any>>> /* Compiler cannot prefer list to Any, tell it explicitly */
			init_elem(Any v)
			: elem(v), type(Elem), l(list())
		{
		}

		init_elem(list && l)
			: l(std::move(l)), type(List)
		{
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

		init_elem(list & l)
			: l(l), type(List)
		{
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}
	};


	list::list(const std::initializer_list<init_elem> & il)
	{
		//self_id = id++;

		//std::cerr << "list id: " << self_id << std::endl;
		//std::cerr << "list init begin" << std::endl;
		for (auto & elem : il)
		{
			switch (elem.type)
			{
			case init_elem::Elem:
				content.push_back(elem.elem);
				break;
			case init_elem::List:
				content.push_back(elem_t(new list(
					std::move(elem.l))));
				break;
			}
		}

		//std::cerr << "list id: " << self_id << std::endl;
		//std::cerr << "list inited, len: " << this->__len__() << std::endl;
	}
}

namespace pythonic
{
	template<typename _ContainerType>
	inline int32_t len(const _ContainerType & container)
	{
		return container.__len__();
	}

	static list make_list(std::initializer_list<init_elem> il)
	{
		return list(il);
	}
}