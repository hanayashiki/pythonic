#pragma once

#include <string>

namespace pythonic
{
	class str
	{
	private:
		std::string content;
	public:
		str(const char * c)
			: content(c)
		{
		}


	};
}