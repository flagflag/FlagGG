//
// 函数
//

#pragma once

namespace FlagGG
{

struct PODFuncStorage
{
	PODFuncStorage()
		: type_(0)
		, funcPtr_(nullptr)
	{

	}

	PODFuncStorage(void* funcPtr)
		: type_(0)
		, funcPtr_(funcPtr)
	{
	}

	template <typename T>
	PODFuncStorage(T classFunc)
		: type_(1)
	{
		new (memory_) T(classFunc);
	}

	void* GetPtr()
	{
		return type_ ? memory_ : funcPtr_;
	}

	PODFuncStorage& operator=(const PODFuncStorage& rhs)
	{
		type_ = rhs.type_;
		memcpy(memory_, rhs.memory_, 24);
		return *this;
	}

private:
	char type_;
	union
	{
		void* funcPtr_;
		char memory_[24];
	};
};

template <typename F>
class FunctionImpl;

template <typename Ret, typename ... Args>
class FunctionImpl<Ret(Args...)>
{
public:
	using FunctionTypeInner = Ret(void*, void*, Args ...);
	using FunctionType = Ret(Args ...);
	using ReturnType = Ret;

	FunctionImpl()
		: func_(nullptr)
		, funcStorage_{}
		, instancePtr_(nullptr)
	{
	}

	FunctionImpl(std::nullptr_t null)
		: func_(nullptr)
		, funcStorage_{}
		, instancePtr_(nullptr)
	{
	}

	FunctionImpl(FunctionType* func)
		: funcStorage_(reinterpret_cast<void*>(func))
		, instancePtr_(nullptr)
	{
		func_ = [](void* unused, void* funcPtr, Args ... args) -> Ret
		{
			return (*reinterpret_cast<FunctionType*>(funcPtr))(std::forward<Args>(args)...);
		};
	}

	template <typename ClassType, typename ClassFuncType>
	FunctionImpl(ClassFuncType classFunc, ClassType* instance)
		: funcStorage_(classFunc)
		, instancePtr_(instance)
	{
		func_ = [](void* instancePtr, void* funcPtr, Args ... args) -> Ret
		{
			auto instanceImpl = reinterpret_cast<ClassType*>(instancePtr);
			auto funcImpl = *reinterpret_cast<ClassFuncType*>(funcPtr);
			return (instanceImpl->*funcImpl)(std::forward<Args>(args)...);
		};
	}

	FunctionImpl(const FunctionImpl& rhs)
	{
		Set(rhs);
	}

	FunctionImpl& operator=(const FunctionImpl& rhs)
	{
		Set(rhs);
		return *this;
	}

	template <typename ...CallArgs>
	Ret operator()(CallArgs&& ... args)
	{
		return func_(instancePtr_, funcStorage_.GetPtr(), std::forward<CallArgs>(args)...);
	}

	operator bool()
	{
		return func_;
	}

protected:
	void Set(const FunctionImpl& rhs)
	{
		func_ = rhs.func_;
		funcStorage_ = rhs.funcStorage_;
		instancePtr_ = rhs.instancePtr_;
	}

private:
	FunctionTypeInner* func_;
	PODFuncStorage funcStorage_;
	void* instancePtr_;
};

template <typename Ret, typename ... Args>
class FunctionImpl<Ret(*)(Args...)> : FunctionImpl<Ret(Args...)> {};

template <typename F>
struct FunctionBinder;

template <typename Ret, typename ClassName, typename ... Args>
struct FunctionBinder<Ret(ClassName::*)(Args...)>
{
	using ClassFuncType = Ret(ClassName::*)(Args...);
	using ClassType = ClassName;

	static FunctionImpl<Ret(Args...)> Bind(ClassFuncType func, ClassType* instance)
	{
		return FunctionImpl<Ret(Args...)>(func, instance);
	}
};

template <typename T>
class Function
{
public:
	using NativeType = T;

	Function()
	{
	}

	Function(std::nullptr_t null) :
		funcWrapper_(null)
	{
	}

	template <typename F>
	Function(F&& func) :
		funcWrapper_(std::forward<F>(func))
	{
	}

	template <typename F, typename I>
	Function(F&& func, I* inst) :
		funcWrapper_(FunctionBinder<F>::Bind(func, inst))
	{
	}

	Function(Function&& rhs)
	{
		funcWrapper_ = std::move(rhs.funcWrapper_);
	}

	Function(const Function& rhs)
	{
		funcWrapper_ = rhs.funcWrapper_;
	}

	Function& operator=(Function&& rhs)
	{
		funcWrapper_ = std::move(rhs.funcWrapper_);
		return *this;
	}

	Function& operator=(const Function& rhs)
	{
		funcWrapper_ = rhs.funcWrapper_;
		return *this;
	}

	template <typename ... Args>
	typename FunctionImpl<T>::ReturnType operator()(Args&& ... args)
	{
		return funcWrapper_(std::forward<Args>(args)...);
	}

private:
	FunctionImpl<T> funcWrapper_;
};

}
