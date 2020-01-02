#pragma once

#include <tuple>
#include <functional>

namespace FlagGG
{
	namespace Core
	{
		template < class F >
		struct FunctionTraits;

		template < class Ret, class ... Args >
		struct FunctionTraits<Ret(Args...)>
		{
			using ReturnType = Ret;
			enum
			{
				arity = sizeof...(Args)
			};
			template < size_t I >
			struct Argument
			{
				static_assert(I < arity, "error: invalid arg index.");
				using Type = typename std::tuple_element<I, std::tuple<Args...>>::type;
			};
		};

		template < class Ret, class ... Args >
		struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {};

		template < class Ret, class ... Args >
		struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret(Args...)> {};

#define CLASS_FUNCTION_TRAITS(suffix) \
		template < class Ret, class ClassName, class ... Args > \
		struct FunctionTraits<Ret(ClassName::*)(Args...) suffix> : FunctionTraits<Ret(Args...)> {};

		CLASS_FUNCTION_TRAITS();
		CLASS_FUNCTION_TRAITS(const);
		CLASS_FUNCTION_TRAITS(volatile);
		CLASS_FUNCTION_TRAITS(const volatile);

		template < class Callanble >
		struct FunctionTraits : FunctionTraits<decltype(&Callanble::operator())> {};
	}
}
