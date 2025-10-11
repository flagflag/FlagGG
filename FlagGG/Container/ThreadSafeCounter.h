//
// 原子int32
//

#pragma once

#include "Core/BaseTypes.h"
#include "Platform/PlatformAtomics.h"

namespace FlagGG
{

class FlagGG_API ThreadSafeCounter
{
public:
	ThreadSafeCounter()
		: counter_(0)
	{

	}

	~ThreadSafeCounter() = default;

	ThreadSafeCounter(Int32 value)
		: counter_(value)
	{

	}

	ThreadSafeCounter(const ThreadSafeCounter& rhs)
	{
		counter_ = rhs.GetValue();
	}

	// 删除拷贝函数
	ThreadSafeCounter& operator=(const ThreadSafeCounter& rhs) = delete;

	// 原子操作，数值+1
	Int32 Increment()
	{
		return PlatformAtomics::InterlockedIncrement(&counter_);
	}

	// 原子操作，数值+amount
	Int32 Add(Int32 amount)
	{
		return PlatformAtomics::InterlockedAdd(&counter_, amount);
	}

	// 原子操作，数值-1
	Int32 Decrement()
	{
		return PlatformAtomics::InterlockedDecrement(&counter_);
	}

	// 原子操作，数值-amount
	Int32 Subtract(Int32 amount)
	{
		return PlatformAtomics::InterlockedAdd(&counter_, -amount);
	}

	// 原子操作，counter_ = value
	Int32 Set(Int32 value)
	{
		return PlatformAtomics::InterlockedExchange(&counter_, value);
	}

	// 原子操作，counter_ = 0
	Int32 Reset()
	{
		return PlatformAtomics::InterlockedExchange(&counter_, 0);
	}

	// 原子操作，获取counter_的值
	Int32 GetValue() const
	{
		return PlatformAtomics::AtomicRead(&const_cast<ThreadSafeCounter*>(this)->counter_);
	}

private:
	volatile Int32 counter_;
};

}
