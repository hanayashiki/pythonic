#pragma once
#include "pythonic_list.h"
#include "pythonic_dict.h"

namespace pythonic
{
	struct init_elem
	{
		elem_value elem;

		template<typename Any, class dummy =
			std::enable_if<
			std::is_same_v<std::remove_cv_t<std::remove_reference_t<Any>>,
				elem_value>>> /* Compiler cannot prefer list to Any, tell it explicitly */
			init_elem(Any v) :
			elem(v)
		{
			std::cerr << "use any initializer" << std::endl;
		}

		init_elem(list && l) :
			elem((container*)new list(std::move(l)))
		{
			std::cerr << "use list initializer" << std::endl;
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

		init_elem(const list & l) :
			elem((container*)new list(l))
		{
			std::cerr << "use list initializer" << std::endl;
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

		init_elem(dict && l) :
			elem((container*)new dict(std::move(l)))
		{
			std::cerr << "use dict initializer" << std::endl;
			//std::cerr << "init list len" << l.__len__() << std::endl;
		}

	};
}