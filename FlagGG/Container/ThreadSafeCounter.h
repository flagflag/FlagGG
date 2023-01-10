//
// ԭ��int32
//

#pragma once

#include "Core/BaseTypes.h"
#include "Platform/PlatformAtomics.h"

namespace FlagGG
{

class ThreadSafeCounter
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

	// ɾ����������
	ThreadSafeCounter& operator=(const ThreadSafeCounter& rhs) = delete;

	// ԭ�Ӳ�������ֵ+1
	Int32 Increment()
	{
		return PlatformAtomics::InterlockedIncrement(&counter_);
	}

	// ԭ�Ӳ�������ֵ+amount
	Int32 Add(Int32 amount)
	{
		return PlatformAtomics::InterlockedAdd(&counter_, amount);
	}

	// ԭ�Ӳ�������ֵ-1
	Int32 Decrement()
	{
		return PlatformAtomics::InterlockedDecrement(&counter_);
	}

	// ԭ�Ӳ�������ֵ-amount
	Int32 Subtract(Int32 amount)
	{
		return PlatformAtomics::InterlockedAdd(&counter_, -amount);
	}

	// ԭ�Ӳ�����counter_ = value
	Int32 Set(Int32 value)
	{
		return PlatformAtomics::InterlockedExchange(&counter_, value);
	}

	// ԭ�Ӳ�����counter_ = 0
	Int32 Reset()
	{
		return PlatformAtomics::InterlockedExchange(&counter_, 0);
	}

	// ԭ�Ӳ�������ȡcounter_��ֵ
	Int32 GetValue() const
	{
		return PlatformAtomics::AtomicRead(&const_cast<ThreadSafeCounter*>(this)->counter_);
	}

private:
	volatile Int32 counter_;
};

}
