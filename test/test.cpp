// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "../pythonic/pythonic.h" // Force rebuilding
#include "pch.h"
#include <iostream>

#define CRTDBG_MAP_ALLOC    
#include <stdlib.h>    
#include <crtdbg.h> 

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif 
#endif  // _DEBUG

int main()
{
	CldLog::SetFilterLevel(CldLog::FilterLevel::Debug);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	list_test_init();
	list_test_append_extend();
	list_test_equal();
	list_test_insert();
	list_test_slice();

	match_test();

	str_test_init();
	list_leak_test();

	ordered_map_test_at();
//	ordered_map_test_find();
	ordered_map_test_insert();
	ordered_map_test_erase();
	
	dict_init_test();

	std::cout << "All good!\n";

	std::cin.get();
}