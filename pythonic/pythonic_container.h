#pragma once

#include <any>
// #include <iostream>
#include <typeinfo>
#include <functional>
#include <memory>

#include "pythonic_iterator.h"

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