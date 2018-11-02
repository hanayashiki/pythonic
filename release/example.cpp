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

struct G;

struct FF
{
	void fuck(G);
};

struct G
{

};

void FF::fuck(G g)
{
}

template<typename K>
struct Big
{
	struct Small
	{
		K a;
	};
};

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
//	map.at(3);

	Fuck3(F());
	Fuck4(4);

	std::unordered_map<int, int> map2{
		{ 1, 2 },
		{ 3, 4 }
	};

	init_elem("1234");

	str("12345");
	str(123);
	str(init_elem("1234"));


	return 0;
}
