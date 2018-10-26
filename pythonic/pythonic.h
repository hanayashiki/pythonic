#pragma once

#include "pythonic_utils_range.h"
#include "pythonic_utils_match.h"
#include "pythonic_container.h"

#include "pythonic_list.h"

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