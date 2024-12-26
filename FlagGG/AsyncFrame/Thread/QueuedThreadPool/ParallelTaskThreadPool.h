//
// 即时性高，线程任务不可被抛弃，通常用于在某个阶段阻塞并行执行任务
//

#pragma once

#include "AsyncFrame/Thread/IQueuedThreadPool.h"
#include "AsyncFrame/Thread/ThreadPool.h"
#include "AsyncFrame/ConditionVariable.h"
#include "Container/LockFreeList.h"

namespace FlagGG
{

class ParallelTaskThreadPool : public IQueuedThreadPool, public IThreadPoolRunnable
{
public:
	ParallelTaskThreadPool();

	~ParallelTaskThreadPool() override;

// Override IQueuedThreadPool

	// 创建线程池
	void SetupThreads(UInt32 numThreads, ThreadPriority threadPriority, const char* threadName) override;

	// 增加线程任务
	void AddThreadTask(IThreadTask* threadTask, ThreadTaskPriority priority) override;


// Override IThreadPoolRunnable

	// 初始化
	void Init(PerThreadFromThreadPool* perThread) override;

	// 运行
	void Run(PerThreadFromThreadPool* perThread) override;

	// 退出
	void Exit(PerThreadFromThreadPool* perThread) override;

// Override Runtime

	// 主动停止
	void Stop() override;

	// 阻塞等待停止
	void WaitForStop() override;

	// 阻塞等待停止，最大等待时间waitTime
	void WaitForStop(UInt32 waitTime) override;

private:
	// 线程池
	SharedPtr<ThreadPool> threadPool_;

	// 任务队列
	StallingTaskQueue<IThreadTask, PLATFORM_CACHE_LINE_SIZE, (Int32)ThreadTaskPriority::Max> taskQueue_;

	struct ThreadEvent
	{
		void SendEvent()
		{
			cond_.Trigger();
		}

		void WaitEvent()
		{
			cond_.Wait(&mutex_);
		}

		Mutex mutex_;
		ConditionVariable cond_;
	};
	// 线程事件
	Vector<ThreadEvent> threadEvents_;
};

}
