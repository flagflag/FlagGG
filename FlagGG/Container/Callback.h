#pragma once

#include "TypeTraits/FunctionTraits.h"
#include <functional>

namespace FlagGG
{

// DelegateArgsTraits

template <typename IDelegateInterface, typename CallbackType, typename F>
class DelegateArgsTraits;

template <typename IDelegateInterface, typename CallbackType, typename Ret, typename ClassName, typename ... Args>
class DelegateArgsTraits<IDelegateInterface, CallbackType, Ret(ClassName::*)(Args...)>
{
public:
	using FunctionType = typename FunctionTraits<decltype(&IDelegateInterface::Invoke)>::FunctionType;
	using ReturnType = typename FunctionTraits<decltype(&IDelegateInterface::Invoke)>::ReturnType;

	class DelegateImpl : public IDelegateInterface
	{
	public:
		DelegateImpl(CallbackType&& callback)
			: callback_(std::forward<CallbackType&&>(callback))
		{

		}

		~DelegateImpl() override
		{

		}

		ReturnType Invoke(Args ... args) throw() override
		{
			return callback_(std::forward<Args>(args)...);
		}

	private:
		std::function<FunctionType> callback_;
	};

	static IDelegateInterface* Callback(CallbackType&& callback)
	{
		return new DelegateImpl(std::forward<CallbackType&&>(callback));
	}
};

// Callback

template <typename IDelegateInterface, typename CallbackType>
IDelegateInterface* Callback(CallbackType&& callback)
{
	return DelegateArgsTraits<IDelegateInterface, CallbackType, decltype(&IDelegateInterface::Invoke)>::Callback(std::forward<CallbackType&&>(callback));
}

}
