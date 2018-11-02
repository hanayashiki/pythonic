#pragma once

#include "pythonic_utils_range.h"
#include "pythonic_utils_match.h"
/* containers */
#include "pythonic_container.h"
#include "pythonic_list.h"
/* init_elem */
#include "pythonic_init_elem.h"
/* implements of initializer_list ctors */
#include "pythonic_list_initializer.h"
#include "pythonic_dict_initializer.h"
#include "pythonic_str_impl.h"

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