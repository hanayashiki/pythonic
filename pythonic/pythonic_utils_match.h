#pragma once

#include <functional>
#include <tuple>
#include <utility>
#include <type_traits>

#include "pythonic_container.h"

namespace pythonic
{

	template<typename T>
	struct function_traits;

	template<typename R, typename ...Args>
	struct function_traits<std::function<R(Args...)>>
	{
		static const size_t nargs = sizeof...(Args);

		typedef R result_type;

		template<size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		};
	};

	template<typename Ret>
	using ret_or_dummy = std::conditional_t<std::is_void<Ret>::value, size_t, Ret>;

	template<typename FuncType>
	struct match_impl_base;

	template<typename Ret, typename First>
	struct match_impl_base<std::function<Ret(First)>>
	{
		typedef container::elem_t elem_t;

		typedef ret_or_dummy<Ret> NonVoidRet;

		template<bool isVoid>
		static inline NonVoidRet match_first
		(elem_t & elem, std::function<Ret(First)> && first, bool & success);

		template<>
		static inline NonVoidRet match_first<false>
			(elem_t & elem, std::function<Ret(First)> && first, bool & success)
		{
			if (elem.isinstance<First>())
			{
				success = true;
				return first(elem.asMut<First>());
			}
			else
			{
				return NonVoidRet();
			}
		}

		template<>
		static inline NonVoidRet match_first<true>
			(elem_t & elem, std::function<Ret(First)> && first, bool & success)
		{

			if (elem.isinstance<First>())
			{
				success = true;
				first(elem.asMut<First>());
			}
			return NonVoidRet();
		}
	};

	template<typename Ret, typename ...FuncTypes>
	struct match_impl;

	template<typename Ret, typename First>
	struct match_impl<std::function<Ret(First)>> : public match_impl_base<std::function<Ret(First)>>
	{
		typedef match_impl_base<std::function<Ret(First)>> _Base;
		typedef typename _Base::NonVoidRet NonVoidRet;

		static NonVoidRet match(_Base::elem_t & elem, std::function<Ret(First)> && function)
		{
			bool success = false;
			
			NonVoidRet result = _Base::match_first<std::is_void_v<Ret>>(
				std::forward<typename _Base::elem_t&>(elem),
				std::forward<std::function<Ret(First)>>(function), 
				success);
			if (success)
			{
				return result;
			}
			else
			{
				return _Base::NonVoidRet();
			}
		}
	};

	template<typename Ret, 
		typename First, typename ...Rest>
	struct match_impl<std::function<Ret(First)>, Rest...> : public match_impl_base<std::function<Ret(First)>>
	{
		typedef match_impl_base<std::function<Ret(First)>> _Base;
		typedef typename _Base::NonVoidRet NonVoidRet;

		static NonVoidRet match(_Base::elem_t & elem, std::function<Ret(First)> && function, Rest &&... rest)
		{
			bool success = false;
			NonVoidRet result = _Base::match_first<std::is_void_v<Ret>>(
				std::forward<typename _Base::elem_t&>(elem),
				std::forward<std::function<Ret(First)>>(function), 
				success);
			if (success)
			{
				return result;
			}
			else
			{
				return match_impl<Rest...>::match(std::forward<typename _Base::elem_t&>(elem), std::forward<Rest>(rest)...);
			}
		}
	};

	template<typename ...Types>
	struct first
	{
		typedef std::tuple_element_t<0, std::tuple<Types...>> value;
	};
	
	template<typename ...Funcs>
	static
	auto match(container::elem_t & elem, Funcs &&... funcs)
	{
		return match(std::forward<container::elem_t&>(elem), std::function(funcs)...);
	}

	template<typename ...Funcs>
	static auto match(container::elem_t && elem, Funcs &&... funcs)
	{
		return match(std::forward<container::elem_t&>(elem), std::function(funcs)...);
	}
	
	template<typename R, typename ...ArgTypes, typename NonVoidRet = ret_or_dummy<R>>
	static
	NonVoidRet
	match(container::elem_t & elem, std::function<R(ArgTypes)> &&... funcs)
	{
		return match_impl<std::function<R(ArgTypes)>...>::match(
			std::forward<container::elem_t&>(elem),
			std::forward<std::function<R(ArgTypes)>>(funcs)...
		);
	}
}