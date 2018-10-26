#include "pch.h"

#include "../pythonic/pythonic.h"
#include <iostream>
#include <string>

#include "tester.h"

using namespace pythonic;

void str_test_init()
{
	str s(1);

	test_utils::tester t(__FUNCTION__);

	t.test() << s;
	t.expected() << "1";
	t.compare();

	str l(list({1, 2, 3}));

	t.test() << l;
	t.expected() << "[1, 2, 3]";
	t.compare();

	str l2(list({ 1, 2, 3, list{ 1, 2, 3 } }));

	t.test() << l2;
	t.expected() << "[1, 2, 3, [1, 2, 3]]";
	t.compare();

	list another({ 1, 2, 3 });
	list yet_another({ 1, 2, 3, another });

	t.test() << str(yet_another);
	t.expected() << "[1, 2, 3, [1, 2, 3]]";
	t.compare();
}