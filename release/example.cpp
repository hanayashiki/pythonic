#include "pythonic.h"
#include <iostream>


int main()
{
	pythonic::list l({ 1 });
	pythonic::any elem(1);

	match(l[0],
	[](int a) {
		std::cout << a;
	}
	);

	match(pythonic::any(1),
		[](int a) {
		std::cout << a;
	}
	);


	return 0;
}