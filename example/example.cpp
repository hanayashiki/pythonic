#include "pch.h"
#include "../release/pythonic.h" // include the header, best if included in the precompiled header
#include <iostream>

int main()
{
	using namespace pythonic; // The namespace to access pythonic containers and utilities.

	/* pythonic::list, pythonic::str */
	list l({ 1, 2, 3,
		list{ 1, "2", 3,
			list{ "fuck", "you", "leatherman" } } });

	/* pythonic conversion to 'str' */
	std::cout << str(l) << std::endl;

	list l2({ "I", "am", "chinese " });

	std::cout << str("|").join(l2) << std::endl;

	/* pythonic::dict */
	dict d{
		{ 123, "456" },
		{ 456, 23333 },
		{ 789, 66666 }
	};

	/* another way to 'str' */
	std::cout << d.__str__() << std::endl;

	std::cin.get();
	return 0;
}
