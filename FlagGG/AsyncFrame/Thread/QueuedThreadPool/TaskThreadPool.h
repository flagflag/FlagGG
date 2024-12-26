//
// 即时性稍低，线程任务可被抛弃
//

#pragma once

#include "AsyncFrame/Thread/IQueuedThreadPool.h"

namespace FlagGG
{

class ThreadPool;

class TaskThreadPool : public IQueuedThreadPool
{
public:
	// 创建线程池
	void SetupThreads(UInt32 numThreads, ThreadPriority threadPriority, const char* threadName) override;

	// 增加线程任务
	void AddThreadTask(IThreadTask* threadTask, ThreadTaskPriority priority) override;

	// 主动停止
	void Stop() override;

	// 阻塞等待停止
	void WaitForStop() override;

	// 阻塞等待停止，最大等待时间waitTime
	void WaitForStop(UInt32 waitTime) override;

private:
	// 线程池
	SharedPtr<ThreadPool> threadPool_;
};

}

