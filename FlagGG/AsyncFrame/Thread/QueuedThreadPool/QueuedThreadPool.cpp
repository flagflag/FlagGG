#include "AsyncFrame/Thread/IQueuedThreadPool.h"
#include "AsyncFrame/Thread/QueuedThreadPool/TaskThreadPool.h"
#include "AsyncFrame/Thread/QueuedThreadPool/ParallelTaskThreadPool.h"

namespace FlagGG
{

IQueuedThreadPool* Allocate(QueuedThreadPoolType type)
{
	switch (type)
	{
	case QueuedThreadPoolType::TaskThreadPool:
		return new TaskThreadPool();
		break;

	case QueuedThreadPoolType::ParallelTaskThreadPool:
		return new ParallelTaskThreadPool();
		break;
	}

	return nullptr;
}

}
