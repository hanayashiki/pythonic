#include "pch.h"

#include "../release/pythonic.h"
#include "tester.h"

void match_test()
{
	test_utils::tester t(__FUNCTION__);

	using namespace pythonic;
	pythonic::any lvalue(1);
	
	match(lvalue,
		[&](int a) { t.test() << a; }
	);
	match(pythonic::any(1),
		[&](int a) { t.test() << a; }
	);

	t.expected() << 1 << 1;

	t.compare();
}