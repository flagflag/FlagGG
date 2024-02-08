#pragma once

#include <functional>

namespace FlagGG
{

template < class FunctionDefine, class InstanceType = void >
class FunctionImpl
{
public:
	using FunctionType = FunctionDefine(InstanceType::*);

	FunctionImpl(FunctionType func, InstanceType* instance) :
		func_(func),
		instance_(instance)
	{ }

	template < class ... Args >
	void operator()(Args&& ... args)
	{
		(instance_->*func_)(std::forward<Args>(args)...);
	}

private:
	InstanceType* instance_;

	FunctionType func_;
};

template < class FunctionDefine >
class FunctionImpl < FunctionDefine >
{
public:
	using FunctionType = FunctionDefine*;

	FunctionImpl(FunctionType func) :
		func_(func)
	{ }

	template < class ... Args >
	void operator()(Args&& ... args)
	{
		(*func_)(std::forward<Args>(args)...);
	}

private:
	FunctionType func_;
};

template < class T >
class Function
{
public:
	typedef Function<T> Type;
	using NativeType = T;

	Function()
	{ }

	Function(std::nullptr_t null) :
		funcWrapper_(null)
	{ }

	Function(const Type& value)
	{
		funcWrapper_ = value.funcWrapper_;
	}

	template < class F >
	Function(F func) :
		funcWrapper_(FunctionImpl<T>(func))
	{ }

	template < class F, class I >
	Function(F func, I* ins) :
		funcWrapper_(FunctionImpl<T, I>(func, ins))
	{ }

	template < class ... Args >
	void operator()(Args&& ... args)
	{
		funcWrapper_(std::forward<Args>(args)...);
	}

private:
	std::function<T> funcWrapper_;
};

}
