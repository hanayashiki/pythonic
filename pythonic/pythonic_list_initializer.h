#pragma once

#include "pythonic_init_elem.h"

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
		//PYC_DEBUG << "list id: " << self_id << std::endl;
		//PYC_DEBUG << "__str__(): this len: " << this->__len__() << std::endl;
		return str("[") + str(", ").join(*this) + str("]");
	}
}