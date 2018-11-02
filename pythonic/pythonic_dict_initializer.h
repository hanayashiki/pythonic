#pragma once
#include "pythonic_dict.h"
#include "pythonic_init_elem.h"

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