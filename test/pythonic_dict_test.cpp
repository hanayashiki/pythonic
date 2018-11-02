#include "pch.h"

#include "../pythonic/pythonic.h"
#include <iostream>
#include <string>

#include "tester.h"

using namespace pythonic;

void dict_init_test()
{
	test_utils::tester t(__FUNCTION__);

	dict d({
		{1, 2},
		{3, 4}
	});

	std::cerr << d.__str__() << std::endl;
	t.test() << d.__str__();
	t.expected() << str("{1: 2, 3: 4}");
	t.compare();
}