//
// 排队任务线程池
//

#pragma once

#include "Core/BaseTypes.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "AsyncFrame/Thread/IThreadTask.h"
#include "AsyncFrame/Thread/UniqueThread.h"

namespace FlagGG
{

enum class QueuedThreadPoolType
{
	TaskThreadPool,
	ParallelTaskThreadPool,
};

class IQueuedThreadPool : public Runtime
{
public:
	virtual ~IQueuedThreadPool() {}

	// 创建线程池
	virtual void SetupThreads(UInt32 numThreads, ThreadPriority threadPriority, const char* threadName) = 0;

	// 增加线程任务
	virtual void AddThreadTask(IThreadTask* threadTask, ThreadTaskPriority priority = ThreadTaskPriority::Normal) = 0;
};

IQueuedThreadPool* Allocate(QueuedThreadPoolType type);

}
