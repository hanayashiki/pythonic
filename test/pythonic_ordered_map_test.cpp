#include "pch.h"
#define private public
#include "tester.h"
#include "../pythonic/pythonic_ordered_map.h"

using namespace pythonic;
using namespace test_utils;

void test_compile_ordered_map()
{
	ordered_map<int, int> o;
	o.insert(1, 2);
	int a, b;
	o.insert(a, b);
	o.insert(a, int(1));
}

void ordered_map_test_find()
{
	//_CrtSetBreakAlloc(178);

	tester t(__FUNCTION__);

	ordered_map<int, int> o;

	using node = ordered_map<int, int>::node;

	o.cur_level = 3;
	/*
	 1
	 1	5	
	 1	5	7	    
	 1	5	7	8   -
	 */

	node * cur = o.head;
	o.head->next[0]->free();
	node * max = node::create_max(0);

	cur->next[0] = node::create(111, 233, 3);
	cur->next[1] = cur->next[0];
	cur->next[2] = cur->next[0];
	cur->next[3] = cur->next[0];

	cur = cur->next[0];
	cur->next[0] = node::create(555, 234, 2);
	cur->next[1] = cur->next[0];
	cur->next[2] = cur->next[0];
	cur->next[3] = max;

	cur = cur->next[0];
	cur->next[0] = node::create(777, 235, 1);
	cur->next[1] = cur->next[0];
	cur->next[2] = max;

	cur = cur->next[0];
	cur->next[0] = node::create(888, 235, 0);
	cur->next[1] = max;

	cur = cur->next[0];
	cur->next[0] = max;

	node * n = o.head;
	while (n != nullptr)
	{
		std::cout << n->kv.first << ", ";
		n = n->next[0];
	}
	std::cout << std::endl;

	bool found = o.find(888, 0, nullptr, false);
	t.test() << found;
	t.expected() << true;
	t.compare();
}

void ordered_map_test_insert()
{
	tester t(__FUNCTION__);
	using node = ordered_map<int, int>::node;

	{
		ordered_map<int, int> o;

		o.insert(0, 1);
		o.insert(1, 2);
		o.insert(2, 3);

		for (auto x : o)
		{
			t.test() << x.first << x.second;
		}

		t.expected() << "011223";

		t.compare();
	}

	{
		ordered_map<int, std::string> o;
		o.insert(0, "fuck");
		o.insert(1, "you");
		o.insert(2, "leatherman");
		for (auto x : o)
		{
			t.test() << x.first << x.second;
		}

		t.expected() << "0fuck1you2leatherman";
		t.compare();
	}
}

void ordered_map_test_at()
{
	tester t(__FUNCTION__);
	using node = ordered_map<int, int>::node;

	auto messup = [](int i) { return 19260827 * i % 17; };

	{
		ordered_map<int, int> o;
		for (auto i : range(1000))
		{
			o.insert(messup(i), 1);
		}
		for (auto i : range(1000))
		{
			t.test() << o.at(messup(i));
			t.expected() << 1;
		}
		t.compare();
	}

	{
		ordered_map<int, int> o;

		o.insert(0, 1);
		o.insert(1, 2);
		o.insert(2, 3);

		t.assert_exception(
			[&]() { o.at(100); }
		);
	}

}

void ordered_map_test_erase()
{
	tester t(__FUNCTION__);
	using node = ordered_map<int, int>::node;

	{
		ordered_map<int, int> o;

		o.insert(233, 1);
		o.insert(1, 2);
		o.insert(2, 3);

		t.test() << o.at(233);
		t.expected() << 1;

		o.erase(233);

		t.assert_exception(
			[&]() { o.at(233); }
		);

		t.assert_exception(
			[&]() { o.erase(233); }
		);

		t.compare();
	}

}