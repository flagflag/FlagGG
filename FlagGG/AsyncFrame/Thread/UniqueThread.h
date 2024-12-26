#pragma once

#include "AsyncFrame/KernelObject/HandleObject.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "Core/Runnable.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Export.h"

#include <functional>

#ifndef _WIN32
#include <pthread.h>
#endif

namespace FlagGG
{

enum class ThreadPriority
{
	Highest,
	High,
	Normal,
	Low,
	Lowest,
	Max,
};

class FlagGG_API UniqueThread : public HandleObject, public Runtime, public RefCounted
{
public:
	UniqueThread(const std::function<void(void)>& threadFunc);

	UniqueThread(std::function<void(void)>&& threadFunc);

	UniqueThread(IRunnable* runnable);

	~UniqueThread() override;

	// 主动停止
	void Stop() override;

	// 阻塞等待停止
	void WaitForStop() override;

	// 阻塞等待停止，最大等待时间waitTime
	void WaitForStop(UInt32 waitTime) override;

	static UInt64 GetCurrentThreadId();

protected:
	void CreateRunnableThread(IRunnable* runnable);

private:
#ifndef _WIN32
	pthread_mutex_t mutex_;
	pthread_cond_t cond_;
#endif

	IRunnable* runnable_;
};

typedef SharedPtr<UniqueThread> UniqueThreadPtr;

}
