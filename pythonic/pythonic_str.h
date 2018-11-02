#pragma once

#include <string>
#include "pythonic_basic_types.h"
#include "pythonic_elem_value.h"
#include "pythonic_utils_match.h"
#include "pythonic_iterator.h"
#include "pythonic_container.h"

namespace std
{
	inline
	std::string to_string(char * s)
	{
		return std::string(s);
	}

	inline
	std::string to_string(const char * s)
	{
		return std::string(s);
	}

	inline
	std::string to_string(const std::string & s)
	{
		return std::string(s);
	}

}

namespace pythonic
{
	struct init_elem;

	class str : public container
	{
	private:
		mutable any cur_char;
		std::string content;
	public:

		inline str(init_elem e);

		str()
		{
		}

		inline str capitalize();

		template<typename Iterable>
		inline str join(const Iterable & iter);

		/* Implements __equal__ */
		virtual bool __equal__(const container & container) const noexcept override
		{
			if (dynamic_cast<const str*>(&container) == nullptr)
			{
				return false;
			}
			else
			{
				return __len__() == container.__len__() && std::equal(begin(), end(), container.begin());
			}
		}

		/* Implements __len__ */
		virtual size_t __len__() const noexcept override
		{
			return content.size();
		}

		/* Implements __str__ */
		inline virtual str __str__() const noexcept override;

		inline str operator + (const str & b);
		inline str operator += (const str & b) { return *this + b; }

		auto & get_content() { return content; }
		const auto & get_content() const { return content; }

		virtual const_iterator begin() const noexcept { return get_iter(content.begin()); }
		virtual const_iterator end() const noexcept { return get_iter(content.end()); }

	private:
		const_iterator get_iter(const std::string::const_iterator & tg) const
		{
			using ContextPtr = const_iterator::ContextPtr;
			using Iter = std::string::const_iterator;

			Iter * iter = new Iter(tg);
			ContextPtr ptr(iter);

			return const_iterator(
				[](ContextPtr ptr) -> void { iterator::visitAs<Iter>(ptr)++; },
				[](ContextPtr ptr, const iterator & that) -> bool { return iterator::visitAs<Iter>(ptr) != iterator::visitAs<Iter>(that.get_context()); },
				[=](ContextPtr ptr) -> const elem_value & { cur_char = any(*iterator::visitAsConst<Iter>(ptr)); return cur_char; },
				ptr
			);
		}

#define TO_STRING(type) [](type a){ return std::to_string(a); }

		_NODISCARD static inline std::string to_string(const pythonic::any & any)
		{
			return match(
				any,
				BASIC_TYPES_LIST(TO_STRING)
				[=](const pythonic::any & t) { return t.get_cont()->__str__().get_content(); });
		}
	};
}

namespace std
{

	inline
	ostream& operator << (ostream& os, const pythonic::str& s)
	{
		os << s.get_content();
		return os;
	}
}