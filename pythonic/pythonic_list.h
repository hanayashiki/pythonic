#pragma once

#include <iostream>

#include <any>
#include <vector>
#include <initializer_list>

#include "pythonic_container.h"
#include "pythonic_elem_value.h"

namespace pythonic 
{
	class list : public container
	{
	public:
		typedef std::vector<elem_t> container_t;
	private:
		container_t content;
	public:
		struct init_elem
		{
			enum Type
			{
				Elem = 1,
				List = 2
			} type;
			elem_value elem;
			const list * l;

			template<typename Any, class dummy = 
				std::enable_if<std::is_same_v<std::remove_cv_t<
					std::remove_reference_t<Any>>, 
					any>>> /* Compiler cannot prefer list to Any, tell it explicitly */
			init_elem(Any v)
				: elem(v), type(Elem)
			{
			}

			init_elem(list && l)
				: l(&l), type(List)
			{
			}

			init_elem(const list & l)
				: l(&l), type(List)
			{
			}

			init_elem(list & l)
				: l(&l), type(List)
			{
			}

			init_elem(const list && l)
				: l(&l), type(List)
			{
			}
		};

		explicit list(std::initializer_list<init_elem> il) 
		{
			std::cout << "use this" << std::endl;
			for (auto & elem : il)
			{
				switch (elem.type)
				{
				case init_elem::Elem:
					content.push_back(elem.elem);
					break;
				case init_elem::List:
					content.push_back(elem_t(new list(*(elem.l))));
					break;	
				}
			}
		}

		list(const container_t & c)
		{
			content = c;
		}

		list(const container_t && c)
		{
			content = c;
		}

		list()
		{
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
			return container_t(content.begin() + a, content.begin() + b);
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