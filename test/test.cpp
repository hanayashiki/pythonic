// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "../pythonic/pythonic.h" // Force rebuilding
#include "pch.h"
#include <iostream>

int main()
{
	list_test_init();
	list_test_append_extend();
	list_test_equal();
	list_test_insert();
	list_test_slice();

	match_test();

	str_test_init();

    std::cout << "All good!\n"; 

	std::cin.get();
}