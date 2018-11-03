#pragma once
#include <any>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <string>

#include "log.h"
#include "pythonic_basic_types.h"
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
			PYC_DEBUG << "elem_value Use any initializer" << std::endl;
		}
		
		elem_value(container * c)
		{
			PYC_DEBUG << "elem_value Use container initializer" << std::endl;
			cont = std::shared_ptr<container>(reinterpret_cast<container*>(c));
		}

		elem_value(list * c) : elem_value((container*)c)
		{
		}

		~elem_value()
		{
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
		inline T as_mut()
		{
			return std::any_cast<T>(this->value);
		}

		template<>
		inline any as_mut<any>()
		{
			return *this;
		}

		template<>
		inline const any & as_mut<const any &>()
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
			// std::cout << value.type().name() << ". " << std::endl;
			return true;
		}

		std::any & get_value() { return value; }
		std::type_index get_type_index() const { return std::type_index(value.type()); }

		auto & get_cont() { return cont; }

		const auto & get_cont() const { return cont; }

#define EQUAL(type) if (that.isinstance<type>()) { return this->as<type>() == that.as<type>(); }

		bool operator == (const elem_value & that) const noexcept
		{
			return match(
				that,
				[=](const any & t)
				{
					if (this->get_type_index() == that.get_type_index())
					{
						if (this->get_cont().get() != nullptr && that.get_cont().get() != nullptr)
						{
							return this->get_cont()->__equal__(*that.get_cont());
						}
						BASIC_TYPES_LIST(EQUAL)
						return false;
					}
					else
					{
						return false;
					}
				}
			);
		}

#define LESS(type) if (that.isinstance<type>()) { return as<type>() < that.as<type>(); }

		bool operator < (const elem_value & that) const noexcept
		{
			return match(
				that,

				[=](const any & t)
				{
					if (this->get_type_index() == that.get_type_index())
					{
						BASIC_TYPES_LIST(LESS)
					}
					else
					{
						return this->get_type_index() < that.get_type_index();
					}
				}
			);
		}
	};

#undef EQUAL
#undef LESS
}
