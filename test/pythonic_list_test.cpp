#include "pch.h"

#include "../pythonic/pythonic_list.h"
#include "../pythonic/pythonic.h"
#include <iostream>
#include <string>

#include "tester.h"

void list_test_init()
{
	test_utils::tester t("list_test_init");

	using namespace pythonic;
	list my_list({ 1, 2, "abc", std::string("efg")});

	for (auto x : my_list)
	{
		if (x.isinstance<int>())
		{
			t.test() << x.as<int>() << "; ";
		}
		else if (x.isinstance<const char *>())
		{
			t.test() << x.as<const char *>() << "; ";
		}
		else if (x.isinstance<std::string>())
		{
			t.test() << x.as<std::string>() << "; ";
		}
	}

	t.expected() << "1; 2; abc; efg; ";
	t.compare();
}

void list_test_append_extend()
{
	test_utils::tester t("list_test_append_extend");

	using namespace pythonic;
	list my_list({ 1, 2, "abc", std::string("efg") });

	my_list.append(123);

	list my_list2({ 1, 2, 3 });

	my_list.extend(my_list2);

	for (const auto & x : my_list)
	{
		if (x.isinstance<int>())
		{
			t.test() << x.as<int>() << "; ";
		}
		else if (x.isinstance<const char *>())
		{
			t.test() << x.as<const char *>() << "; ";
		}
		else if (x.isinstance<std::string>())
		{
			t.test() << x.as<std::string>() << "; ";
		}
	}

	t.expected() << "1; 2; abc; efg; 123; 1; 2; 3; ";
	t.compare();

	t.assert_exception([&]() {
		my_list2.extend(my_list2);
	});

}

void list_test_equal()
{
	test_utils::tester t("list_test_equal");

	using namespace pythonic;
	list my_list1, my_list2;
	my_list1.append(1);
	my_list1.append(2).append(3);
	my_list2.extend({ 1, 2, 3 });

	t.test() << (my_list1 == my_list2);
	t.expected() << true;

	t.compare();

	my_list2.extend({ 1, 2, 3 });

	t.clear();
	t.test() << (my_list1 == my_list2);
	t.expected() << false;

	t.compare();

	my_list1 = list({ 1, 2, 3, list({1, 2, 3}) });
	my_list2 = list({ 1, 2, 3, list{1, 2, 3} });

	t.clear();
	t.test() << (my_list1 == my_list2);
	t.expected() << true;
	t.compare();

}

void list_test_insert()
{
	test_utils::tester t("list_test_insert");

	using namespace pythonic;

	list my_list1 = make_list({ list({1, 2, 3}) });

	my_list1.insert(0, 3).insert(0, 2).insert(0, 1);
	auto my_list2 = list({ 1, 2, 3, list({1, 2, 3}) });

	std::cerr << my_list1.__len__() << std::endl;
	std::cerr << my_list2.__len__() << std::endl;

	t.test() << (my_list1 == my_list2);
	t.expected() << true;
	t.compare();
}

void list_test_slice()
{
	test_utils::tester t("list_test_slice");

	using namespace pythonic;

	list my_list1 = make_list({ 1, 2, 3, std::string("abc"), 4 });

	auto my_list2 = list({ 3, std::string("abc") });

	t.test() << (my_list1(2, 4) == my_list2);
	t.expected() << true;
	t.compare();
}