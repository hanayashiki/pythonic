#include "pythonic.h"
#include <vector>
#include <iostream>

struct F
{
	int a;
	F()
	{
		a = 0;
		std::cout << "F built" << std::endl;
	}

	F(F&)
	{
		std::cout << "F lcopy" << std::endl;
	}

	~F()
	{
		std::cout << "F destroyed" << std::endl;
	}
};

void Fuck1(F f)
{
	f.a = 1;
	std::cout << f.a << std::endl;
	std::cout << "Fuck 1 called" << std::endl;
}

void Fuck2(F && f)
{
	std::cout << "Fuck 2 called" << std::endl;
}


int main()
{
	using namespace pythonic;

	list l({ 1, 2, 3, 
		list{ 1, "2", 3, 
			list{ "fuck", "you", "leatherman" } } });

	std::cout << str(l);

	std::cin.get();
	return 0;
}
