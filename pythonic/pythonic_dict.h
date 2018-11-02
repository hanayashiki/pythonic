#pragma once

#include "pythonic_container.h"
#include "pythonic_ordered_map.h"
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