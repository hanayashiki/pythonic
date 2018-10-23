#pragma once
#include <any>
#include <iostream>
#include <typeinfo>
#include <string>

#include "pythonic_container.h"
#include "pythonic_utils_match.h"


namespace pythonic
{
	class list;

	class elem_value
	{
		std::any value;
		std::shared_ptr<container> cont;
	public:
		template<class ...Args>
		elem_value(Args... args) :
			value(std::forward<Args>(args)...)
		{
		}
		
		elem_value(list * c)
		{
			cont = std::shared_ptr<container>(reinterpret_cast<container*>(c));
		}

		template<typename T>
		inline T as() const
		{
			return std::any_cast<T>(this->value);
		}

		template<>
		inline any as<any>() const
		{
			return *this;
		}

		template<>
		inline const any & as<const any &>() const
		{
			return *this;
		}

		template<typename T>
		inline T asMut()
		{
			return std::any_cast<T>(this->value);
		}

		template<>
		inline any asMut<any>()
		{
			return *this;
		}

		template<>
		inline const any & asMut<const any &>()
		{
			return *this;
		}
		
		static
		inline any remove_const(const elem_value & v)
		{
			elem_value v1 = v;
			return v1;
		}

		template<typename T>
		inline bool isinstance() const
		{
			return value.type() == typeid(T);
		}

		template<>
		inline bool isinstance<any>() const
		{
			return true;
		}

		template<>
		inline bool isinstance<const any &>() const
		{
			std::cout << value.type().name() << ". " << std::endl;
			return true;
		}

		std::any & get_value()
		{
			return value;
		}

#define COMPARE(type) [=](type t) { \
			std::cerr << "comparing: " << as<type>() << ", " << that.as<type>() << std::endl;\
			return as<type>() == that.as<type>(); \
		}

		bool operator == (const elem_value & that) const noexcept
		{
			auto that0 = remove_const(that);
			return match(
				that0,
				COMPARE(int8_t), COMPARE(int16_t), COMPARE(int32_t), COMPARE(int64_t),
				COMPARE(int8_t*), COMPARE(int16_t*), COMPARE(int32_t*), COMPARE(int64_t*),
				COMPARE(uint16_t), COMPARE(uint32_t), COMPARE(uint32_t), COMPARE(uint64_t),
				COMPARE(uint16_t*), COMPARE(uint32_t*), COMPARE(uint32_t*), COMPARE(uint64_t*),
				COMPARE(char), COMPARE(char16_t), COMPARE(char32_t), COMPARE(wchar_t),
				COMPARE(char*), COMPARE(char16_t*), COMPARE(char32_t*), COMPARE(wchar_t*),

				COMPARE(std::string), 

				[=](const any & t)
				{
					return *cont == *(t.cont);
				}
			);
		}
	};

#undef COMPARE
}
