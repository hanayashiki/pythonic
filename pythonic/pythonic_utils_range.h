#pragma once

#include <iostream>
#include "pythonic_container.h"

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