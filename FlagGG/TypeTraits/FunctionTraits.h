#pragma once

#include <tuple>
#include <functional>

namespace FlagGG
{

template <typename F>
struct FunctionTraits;

template <typename Ret, typename ... Args>
struct FunctionTraits<Ret(Args...)>
{
	using FunctionType = Ret(Args...);
	using ReturnType = Ret;
	using ArgumentTuple = std::tuple<Args...>;
	enum
	{
		Arity = sizeof...(Args)
	};
	template <size_t I>
	struct Argument
	{
		static_assert(I < Arity, "error: invalid arg index.");
		using Type = typename std::tuple_element<I, ArgumentTuple>::type;
	};
};

template <typename Ret, typename ... Args>
struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {};

template <typename Ret, typename ... Args>
struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret(Args...)> {};

template <typename Ret, typename ClassName, typename ... Args>
struct FunctionTraits<Ret(ClassName::*)(Args...)> : FunctionTraits<Ret(Args...)> {};

}
