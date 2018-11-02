#include "pythonic.h"
#include <vector>
#include <unordered_map>
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

void Fuck3(const F & f)
{
	std::cout << "Fuck 3 called" << std::endl;
}

void Fuck4(const int & a)
{
	std::cout << "Fuck 4 called" << std::endl;
}

int main()
{
	using namespace pythonic;

	list l({ 1, 2, 3, 
		list{ 1, "2", 3, 
			list{ "fuck", "you", "leatherman" } } });

	std::cout << str(l) << std::endl;

	list l2({ "I", "am", "chinese " });

	std::cout << str("|").join(l2) << std::endl;

	std::cin.get();

	std::unordered_map<int, int> map;
	map.at(3);

	Fuck3(F());
	Fuck4(4);

	return 0;
}
