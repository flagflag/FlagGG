#include "ThreadPool.h"
#include "Utility/SystemHelper.h"
#include "Core/CryAssert.h"

namespace FlagGG
{

#define THREAD_POOL_MAX_SIZE 64

class PerThreadRunnable : public IRunnable
{
public:
	PerThreadRunnable(PerThreadFromThreadPool* currentThread, IThreadPoolRunnable* threadPoolRunnable)
		: currentThread_(currentThread)
		, threadPoolRunnable_(threadPoolRunnable)
	{

	}

	void Init() override
	{
		threadPoolRunnable_->Init(currentThread_);
	}

	void Run() override
	{
		threadPoolRunnable_->Run(currentThread_);
	}

	void Stop() override
	{

	}

	void Exit() override
	{
		threadPoolRunnable_->Exit(currentThread_);
	}

private:
	PerThreadFromThreadPool* currentThread_;

	IThreadPoolRunnable* threadPoolRunnable_;
};

PerThreadFromThreadPool::PerThreadFromThreadPool(UInt32 index, IThreadPoolRunnable* runnable)
	: UniqueThread(new PerThreadRunnable(this, runnable))
	, index_(index)
{
}

PerThreadFromThreadPool::~PerThreadFromThreadPool() = default;

ThreadPool::ThreadPool(UInt32 numThreads, ThreadPriority threadPriority, IThreadPoolRunnable* runnable)
{
	CRY_ASSERT(numThreads <= THREAD_POOL_MAX_SIZE);

	threads_.Resize(numThreads);

	for (USize i = 0; i < numThreads; ++i)
	{
		threads_[i] = MakeShared<PerThreadFromThreadPool>(i, runnable);
	}
}

ThreadPool::~ThreadPool() = default;

void ThreadPool::Stop()
{
	for (USize i = 0; i < threads_.Size(); ++i)
	{
		threads_[i]->Stop();
	}
};

void ThreadPool::WaitForStop()
{
	for (USize i = 0; i < threads_.Size(); ++i)
	{
		threads_[i]->WaitForStop();
	}
};

void ThreadPool::WaitForStop(UInt32 waitTime)
{
	UInt32 startTime = Tick();
	UInt32 deltaTime = 0;

	for (USize i = 0; i < threads_.Size(); ++i)
	{
		threads_[i]->WaitForStop(waitTime - deltaTime);
		deltaTime = Tick() - startTime;
		if (deltaTime >= waitTime)
			break;
	}
}

}