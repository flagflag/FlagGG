#include "ParallelTaskThreadPool.h"

namespace FlagGG
{

ParallelTaskThreadPool::ParallelTaskThreadPool()
{

}

ParallelTaskThreadPool::~ParallelTaskThreadPool()
{

}

void ParallelTaskThreadPool::SetupThreads(UInt32 numThreads, ThreadPriority threadPriority, const char* threadName)
{
	if (!threadPool_)
	{
		threadEvents_.Resize(numThreads);
		threadPool_ = new ThreadPool(numThreads, threadPriority, this);
	}
}

void ParallelTaskThreadPool::AddThreadTask(IThreadTask* threadTask, ThreadTaskPriority priority)
{
	Int32 threadIndexToWake = taskQueue_.Push(threadTask, (UInt32)priority);
	if (threadIndexToWake >= 0)
	{
		threadEvents_[threadIndexToWake].SendEvent();
	}
}

void ParallelTaskThreadPool::Init(PerThreadFromThreadPool* perThread)
{

}

void ParallelTaskThreadPool::Run(PerThreadFromThreadPool* perThread)
{
	const UInt32 threadIndex = perThread->GetIndexInThreadPool();
	auto& currentEvent = threadEvents_[threadIndex];

	while (true)
	{
		IThreadTask* threadTask = taskQueue_.Pop(threadIndex, true, (UInt32)ThreadTaskPriority::Max);
		if (threadTask)
		{
			threadTask->Run();
		}
		else
		{
			currentEvent.WaitEvent();
		}
	}
}

void ParallelTaskThreadPool::Exit(PerThreadFromThreadPool* perThread)
{

}

void ParallelTaskThreadPool::Stop()
{
	threadPool_->Stop();
}

void ParallelTaskThreadPool::WaitForStop()
{
	threadPool_->WaitForStop();
}

void ParallelTaskThreadPool::WaitForStop(UInt32 waitTime)
{
	threadPool_->WaitForStop(waitTime);
}

}
