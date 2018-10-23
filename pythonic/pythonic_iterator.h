#pragma once

#include <any>
// #include <iostream>
#include <typeinfo>
#include <functional>
#include <memory>

namespace pythonic
{
	class elem_value;

	class iterator
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef elem_value value_type;
		typedef size_t difference_type;
		typedef elem_value * pointer;
		typedef elem_value & reference;

		typedef std::shared_ptr<void> ContextPtr;
		typedef std::function<void(ContextPtr)> Increment;
		typedef std::function<bool(ContextPtr, const iterator &)> NotEqual;
		typedef std::function<elem_value &(ContextPtr)> Visit;
	protected:
		ContextPtr context;
		const Increment incre;
		const NotEqual not_equal;
		const Visit visit;
	public:
		iterator(Increment incre, NotEqual not_equal, Visit visit, const ContextPtr & context) :
			incre(incre), not_equal(not_equal), visit(visit), context(context)
		{}

		void operator++ ()
		{
			incre(context);
		}

		elem_value & operator* ()
		{
			return visit(context);
		}

		bool operator != (const iterator & that) const noexcept
		{
			return not_equal(context, that);
		}

		bool operator == (const iterator & that) const noexcept
		{
			return !not_equal(context, that);
		}

		const ContextPtr & get_context() const noexcept
		{
			return context;
		}

		template<typename Type>
		static Type & visitAs(const ContextPtr & ptr)
		{
			return *reinterpret_cast<Type*>(ptr.get());
		}

		template<typename Type>
		static const Type & visitAsConst(const ContextPtr & ptr)
		{
			return *reinterpret_cast<Type*>(ptr.get());
		}

		template<typename Iter>
		static inline iterator from_iter(const Iter & iter)
		{
			Iter * p_iter = new Iter(iter);
			ContextPtr ptr(p_iter);

			return iterator(
				[](ContextPtr ptr) -> void { visitAs<Iter>(ptr)++; },
				[](ContextPtr ptr, const iterator & that) -> bool { return visitAs<Iter>(ptr) != visitAs<Iter>(that.get_context()); },
				[](ContextPtr ptr) -> elem_value & { return *visitAs<Iter>(ptr); },
				ptr
			);
		}
	};

	class const_iterator : public iterator
	{
	protected:
		typedef std::function<const elem_value &(ContextPtr)> ConstVisit;
	private:
		const ConstVisit const_visit;
	public:
		const_iterator(Increment incre, NotEqual not_equal, ConstVisit visit, const ContextPtr & context) :
			iterator(incre, not_equal, nullptr, context), const_visit(visit)
		{
		}

		const elem_value & operator* () const
		{
			return const_visit(context);
		}

		template<typename Iter>
		static inline const_iterator from_const_iter(const Iter & iter)
		{
			Iter * p_iter = new Iter(iter);
			ContextPtr ptr(p_iter);
			return const_iterator(
				[](ContextPtr ptr) -> void { visitAs<Iter>(ptr)++; },
				[](ContextPtr ptr, const iterator & that) -> bool { return visitAs<Iter>(ptr) != visitAs<Iter>(that.get_context()); },
				[](ContextPtr ptr) -> const elem_value & { return *visitAsConst<Iter>(ptr); },
				ptr
			);
		}
	};
}