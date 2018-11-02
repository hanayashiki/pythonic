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
				[](ContextPtr ptr) -> void { ++visitAs<Iter>(ptr); },
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
				[](ContextPtr ptr) -> void { ++visitAs<Iter>(ptr); },
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
		using elem_t = ::pythonic::elem_value;

		virtual bool __equal__(const container & that) const noexcept = 0;
		virtual size_t __len__() const noexcept = 0;
		virtual str __str__() const noexcept = 0;

		virtual const_iterator begin() const noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		virtual ~container() {}

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
/* containers */

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
			std::cerr << "elem_value Use any initializer" << std::endl;
		}
		
		elem_value(container * c)
		{
			std::cerr << "elem_value Use container initializer" << std::endl;
			cont = std::shared_ptr<container>(reinterpret_cast<container*>(c));
		}

		elem_value(list * c) : elem_value((container*)c)
		{
		}

		~elem_value()
		{
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
		inline T as_mut()
		{
			return std::any_cast<T>(this->value);
		}

		template<>
		inline any as_mut<any>()
		{
			return *this;
		}

		template<>
		inline const any & as_mut<const any &>()
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

#define LESS(type) [=](type t) { \
			return as<type>() < that.as<type>(); \
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

		bool operator < (const elem_value & that) const noexcept
		{
			return match(
				that,
				BASIC_TYPES_LIST(LESS)

				[=](const any & t)
				{
					return false;
					//return *cont < *(t.cont);
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
	struct init_elem;

	class str : public container
	{
	private:
		mutable any cur_char;
		std::string content;
	public:

		inline str(init_elem e);

		str()
		{
		}

		inline str capitalize();

		template<typename Iterable>
		inline str join(const Iterable & iter);

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
		inline virtual str __str__() const noexcept override;

		inline str operator + (const str & b);
		inline str operator += (const str & b) { return *this + b; }

		auto & get_content() { return content; }
		const auto & get_content() const { return content; }

		virtual const_iterator begin() const noexcept { return get_iter(content.begin()); }
		virtual const_iterator end() const noexcept { return get_iter(content.end()); }

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
				[=](const pythonic::any & t) { return t.get_cont()->__str__().get_content(); });
		}
	};
}

namespace std
{

	inline
	ostream& operator << (ostream& os, const pythonic::str& s)
	{
		os << s.get_content();
		return os;
	}
}

namespace pythonic 
{ 
	struct init_elem;

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

		list & operator = (const list & l)
		{
			//self_id = l.self_id;
			content = l.content;
		}

		list & operator = (list && l)
		{
			//self_id = l.self_id;
			content = std::move(l.content);
		}

		explicit list(container_t c)
		{
			content = std::move(c);
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
		inline virtual str __str__() const noexcept override;

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

}
/* init_elem */
#pragma once

#pragma once


#pragma once
#pragma warning( disable : 4200 ) 
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <vector>
#include <random>

namespace pythonic
{
	template<typename K, typename V, typename Compare>
	class ordered_map_items;

	template<typename K, typename V, typename Compare = std::less<K>>
	struct ordered_map_node
	{
		using node = ordered_map_node<K, V>;

		class const_iterator
		{
		private:
			node * n;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = const K;
			using difference_type = size_t;
			using pointer = const K *;
			using reference = const K &;

			const_iterator(node * n)
				: n(n)
			{
			}

			const_iterator operator++ () noexcept
			{
				n = n->next[0];
				return *this;
			}

			reference operator* () const noexcept
			{
				return n->kv.first;
			}

			bool operator != (const const_iterator & that) const noexcept
			{
				return n != that.n;
			}

			bool operator == (const const_iterator & that) const noexcept
			{
				return !(*this != that);
			}
		};

		std::pair<const K, V> kv;
		enum class MinMaxMark : char
		{
			Min,
			Max,
			None
		} mark = MinMaxMark::None;
		node* next[];
		
		ordered_map_node(K && key, V && value)
			: kv(std::move(key), std::move(value))
		{
		}

		ordered_map_node(const K & key, const V & value)
			: kv(key, value)
		{
		}

		~ordered_map_node() {}

		static node * malloc(size_t n)
		{
			auto size = offsetof(node, next) + sizeof(node*) * (n + 1);
			auto ptr = (node*)::malloc(size);
			memset(ptr, 0, size);
			return ptr;
		}

		static node * create(K && key, V && value, size_t n)
		{
			node * mem = malloc(n);
			return new (mem) ordered_map_node(std::move(key), std::move(value));
		}

		static node * create(const K & key, const V & value, size_t n)
		{
			node * mem = malloc(n);
			return new (mem) ordered_map_node(key, value);
		}

		static node * create(MinMaxMark min_max, size_t n)
		{
			node * mem = malloc(n);
			mem->mark = min_max;
			return mem;
		}

		static node * create_min(size_t n)
		{
			return create(MinMaxMark::Min, n);
		}

		static node * create_max(size_t n)
		{
			return create(MinMaxMark::Max, n);
		}

		void free()
		{
			if (mark == MinMaxMark::None)
			{
				std::cerr << "dtor called" << std::endl;
				this->~ordered_map_node();
			}
			else
			{
				std::cerr << "mark: " << int(mark);
				std::cerr << ", dtor not called" << std::endl;
			}
			::free(this);
		}

		void free_all()
		{
			node * cur = this;
			while (cur != nullptr)
			{
				node * next = cur->next[0];
				cur->free();
				cur = next;
			}
		}

		int key_compare(const K & key) noexcept
		{
			if (this->mark == MinMaxMark::Min)
			{
				return -1;
			}
			if (this->mark == MinMaxMark::Max)
			{
				return 1;
			}
			else 
			{
				return Compare()(kv.first, key) ? -1 :
					Compare()(key, kv.first) ? 1 :
					0;
			}
		}

		void insert_at(size_t next_slot, node * node) noexcept
		{
			node->next[next_slot] = next[next_slot];
			next[next_slot] = node;
		}

		void disconnect_at(size_t next_slot) noexcept
		{
			next[next_slot] = next[next_slot]->next[next_slot];
		}
	};

	template<typename K, typename V, typename Compare = std::less<K>>
	class ordered_map
	{
	public:
		using node = ordered_map_node<K, V, Compare>;
		using const_iterator = typename node::const_iterator;
		using value_type = std::pair<const K, V>;
		using size_type = size_t;
	private:
		node * head;
		node * rear;
		size_t count = 0;

		size_t cur_level = 0;
		const size_t max_level;

		size_t random_level()
		{
			size_t level = 0;
			while (rand() % 2 == 1)
			{
				level++;
			}
			return std::min(level, max_level);
		}

	public:

		ordered_map(size_t max_level = 4) : 
			max_level(max_level),
			rear(node::create_max(0)),
			head(node::create_min(max_level))
		{
			srand(0);
			for (size_t i = 0; i <= max_level; i++)
				head->next[i] = rear;
		}

		ordered_map(const ordered_map &) = delete;
		ordered_map(ordered_map && map) :
			head(map.head),
			rear(map.rear),
			max_level(map.max_level)
		{
		}

		ordered_map & operator = (const ordered_map &) = delete;
		ordered_map & operator = (ordered_map && t)
		{
			head->free_all();
			head = t.head;
			rear = t.rear;
			t.head = nullptr;
			t.rear = nullptr;
			max_level = t.max_level;
		}

		size_t size() const noexcept { return count; }

		ordered_map<K, V> & insert(const K & key, const V & val)
		{
			auto level = random_level();
			return insert(std::forward<const K &>(key), std::forward<const V &>(val), level);
		}

		ordered_map<K, V> & insert(const K & key, const V & val, size_t level)
		{
			this->cur_level = std::max(this->cur_level, level);

			node ** prev = new node*[level + 1];
			node * n = find(key, level, prev, false);
			if (n != nullptr)
			{
				n->kv.second = val;
			}
			else
			{
				count++;
				node * new_node = node::create(key, val, level);
				for (size_t i = 0; i <= level; i++)
				{
					prev[i]->insert_at(i, new_node);
				}
			}
			delete prev;
			return *this;
		}

		ordered_map<K, V> & erase(const K & key)
		{
			node ** prev = new node*[this->cur_level + 1];
			memset(prev, 0, this->cur_level + 1);

			node * n = find(key, this->cur_level, prev, true);

			if (n != nullptr)
			{
				count--;
				for (size_t i = 0; i < this->cur_level; i++)
				{
					if (prev[i]->next[i] == n)
					{
						prev[i]->disconnect_at(i);
					}
				}
				n->free();
			}
			else
			{
				delete prev;
				throw std::exception("invalid ordered_map<K, V> key");
			}
			delete prev;
			return *this;
		}

		V & at(const K & key)
		{
			node * n = find(key, 0, nullptr, false);
			if (n != nullptr)
			{
				return n->kv.second;
			}
			else
			{
				throw std::exception("invalid ordered_map<K, V> key");
			}
		}

		V & operator[](const K & key) noexcept
		{
			node * n = find(key, 0, nullptr, false);
			return n->kv.second;
		}

		/*
		 node * return : if found, the node; else nullptr.
		 key : the const ref to the key to search
		 node_level : the level of the key; it is used for insert
		 prev : a pointer to an array to save the previous nodes of the key's node
		 record_prev : whether to save to 'prev'
		 */
		node * find(const K & key, size_t node_level, node ** prev, bool record_prev) noexcept
		{
			size_t level = cur_level;
			node * cur = head; // Starting from head node, which is -inf
			node * ans = nullptr;
			while (cur != nullptr)
			{
				int cmp = cur->key_compare(key); // Compare with current key
				//std::cout << "comparing 1 with: " << cur->kv.first << "@" << level << "cmp: " << cmp << std::endl;
				if (cmp == 0)
				{
					ans = cur; // If equal, the key is found
					break;
				}
				else
				{
					node * next = cur->next[level];
					int cmp = next->key_compare(key); // Notice that the key has not been found on cur or before, we compare the next key
					//std::cout << "cur_key: " << cur->kv.first << std::endl;
					//std::cout << "comparing 2 with: " << next->kv.first << "@" << level << "cmp: " << cmp << std::endl;
					if (cmp < 0)
					{
						//std::cout << "move to next" << std::endl;
						cur = cur->next[level]; // If the key is smaller than our key, it means we can safely forward on the current level
					}
					else 
					{
						if (cmp == 0 && !record_prev)
						{
							cur = cur->next[level];
						}
						else
						{
							if (cmp == 0)
							{
								ans = next;
							}
							//std::cout << "level -= 1; level = " << level << std::endl;
							if (level <= node_level && prev != nullptr)
							{
								prev[level] = cur;
							}
							if (level == 0) break;
							level -= 1;
						}
					}
				}
			}

			return ans;
		}

		const_iterator begin() const
		{
			return const_iterator(head->next[0]);
		}

		const_iterator end() const
		{
			return const_iterator(rear);
		}

		ordered_map_items<K, V, Compare> items() const noexcept;

		~ordered_map()
		{
			head->free_all();
		}
	};

	template<typename K, typename V, typename Compare = std::less<K>>
	class ordered_map_items
	{
		using node = ordered_map_node<K, V, Compare>;
		node * head;
		node * rear;
	public:
		ordered_map_items(node * head, node * rear) :
			head(head), rear(rear)
		{
		}

		class const_iterator
		{
		private:
			node * n;
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef const std::pair<const K, V> value_type;
			typedef size_t difference_type;
			typedef const std::pair<const K, V> * pointer;
			typedef const std::pair<const K, V> & reference;

			const_iterator(node * n) : n(n) {}

			void operator ++ () noexcept { n = n->next[0]; }
			reference operator* () const noexcept { return n->kv; }
			bool operator != (const const_iterator & that) const noexcept { return n != that.n; }
			bool operator == (const const_iterator & that) const noexcept { return !(*this != that); }
		};

		const_iterator begin() const { return const_iterator(head->next[0]); }
		const_iterator end() const { return const_iterator(rear); }
	};

	template<typename K, typename V, typename Compare>
	inline ordered_map_items<K, V, Compare> ordered_map<K, V, Compare>::items() const noexcept
	{
		return ordered_map_items<K, V, Compare>(this->head, this->rear);
	}
}
#include <utility>

namespace pythonic
{
	struct init_elem;

	class dict : public container
	{
	private:
		typedef ordered_map<elem_value, elem_value> container_t;
		typedef container_t::value_type value_type;
		container_t content;
	public:
		dict() {}
		inline explicit dict(const std::initializer_list<std::pair<init_elem, init_elem>> & il);

		virtual const_iterator begin() const noexcept
		{
			return const_iterator::from_const_iter(content.begin());
		}

		virtual const_iterator end() const noexcept
		{
			return const_iterator::from_const_iter(content.end());
		}

		/* Implements __str__ */
		inline virtual str __str__() const noexcept override;

		/* Implements __len__ */
		virtual size_t __len__() const noexcept override { return content.size(); }

		/* Implements __equal__ */
		virtual bool __equal__(const container & container) const noexcept override
		{
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
		}

	};
}

namespace pythonic
{
	struct init_elem
	{
		elem_value elem;

		template<typename Any, class dummy =
			std::enable_if<
			std::is_same_v<std::remove_cv_t<std::remove_reference_t<Any>>,
				elem_value>>> /* Compiler cannot prefer list to Any, tell it explicitly */
			init_elem(Any v) :
			elem(v)
		{
			std::cerr << "use any initializer" << std::endl;
		}

		init_elem(list && l) :
			elem((container*)new list(std::move(l)))
		{
			std::cerr << "use list initializer" << std::endl;
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

		init_elem(const list & l) :
			elem((container*)new list(l))
		{
			std::cerr << "use list initializer" << std::endl;
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

		init_elem(dict && l) :
			elem((container*)new dict(std::move(l)))
		{
			std::cerr << "use dict initializer" << std::endl;
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

	};
}
/* implements of initializer_list ctors */
#pragma once



namespace pythonic
{
	list::list(const std::initializer_list<init_elem> & il)
	{
		for (const auto & e : il)
		{
			content.push_back(e.elem);
		}
	}

	str list::__str__() const noexcept
	{
		//std::cerr << "list id: " << self_id << std::endl;
		//std::cerr << "__str__(): this len: " << this->__len__() << std::endl;
		return str("[") + str(", ").join(*this) + str("]");
	}
}
#pragma once



namespace pythonic
{
	inline dict::dict(const std::initializer_list<std::pair<init_elem, init_elem>>& il)
	{
		for (const auto & pair : il)
		{
			this->content.insert(pair.first.elem, pair.second.elem);
		}
	}

	inline str dict::__str__() const noexcept
	{
		str res("{");
		bool first = true;
		std::cerr << "dict size: " << content.size();
		if (content.size() > 0)
		{
			res = res + str((*(content.items().begin())).first) + str(": ") + str((*(content.items().begin())).second);
			for (const auto & pair : content.items())
			{
				if (first == false)
				{
					res = res + str(", ") + str(pair.first) + str(": ") + str(pair.second);
				}
				else
				{
					first = false;
				}
			}
		}
		res = res + str("}");
		return res;
	}
}
#pragma once




namespace pythonic
{
	inline str::str(init_elem e) :
		content(to_string(e.elem))
	{
	}

	str str::capitalize()
	{
		return str("");
	}

	template<typename Iterable>
	str str::join(const Iterable & iter)
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

	str str::__str__() const noexcept
	{
		return str("'") + *this + str("'");
	}

	str str::operator + (const str & b)
	{
		return str(content + b.content);
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