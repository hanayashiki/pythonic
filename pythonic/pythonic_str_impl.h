#pragma once

#include "pythonic_str.h"
#include "pythonic_init_elem.h"

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
		// PYC_DEBUG << "Use join" << std::endl;
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