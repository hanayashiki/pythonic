# pythonic
C++ toy APIs that simulate python

## Usage

This is a header-only library to boost your programming with some python-like apis.
Just copy [pythonic.h](https://github.com/hanayashiki/pythonic/blob/master/release/pythonic.h) to your content and

```
#include "pythonic.h" // include the header, best if included in the precompiled header
#include <iostream>

int main()
{
	using namespace pythonic; // The namespace to access pythonic containers and utilities.

	list l({ 1, 2, 3, 
		list{ 1, "2", 3, 
			list{ "fuck", "you", "leatherman" } } });

	std::cout << str(l) << std::endl;

	list l2({ "I", "am", "chinese " });

	std::cout << str("|").join(l2) << std::endl;

	std::cin.get();
	return 0;
}
```
And you get
```
[1, 2, 3, [1, 2, 3, [fuck, you, leatherman]]]
I|am|chinese
```

The library in under implementation and not fully debugged and featured. Let's make C++ a dynamic language :(
