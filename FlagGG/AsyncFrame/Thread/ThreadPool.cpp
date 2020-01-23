#include "ThreadPool.h"
#include "Utility/SystemHelper.h"

#include <assert.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
#define THREAD_POOL_MAX_SIZE 64

			ThreadPool::ThreadPool(UInt32 threadCount)
			{
				assert(threadCount <= THREAD_POOL_MAX_SIZE);

				for (Size i = 0; i < threadCount; ++i)
				{
					threads_.Push(SharedThreadPtr(new SharedThread));
				}
			}

			void ThreadPool::Add(ThreadTask task_func)
			{
				Size index = 0;
				UInt32 minWaitingTime = INT_MAX;

				for (Size i = 0; i < threads_.Size(); ++i)
				{
					UInt32 waitingTime = threads_[i]->WaitingTime();
					if (waitingTime < minWaitingTime)
					{
						index = i;
						minWaitingTime = waitingTime;
					}
				}

				if (threads_.Size() > 0)
				{
					threads_[index]->Add(task_func);
				}
			}

			void ThreadPool::Start()
			{
				for (Size i = 0; i < threads_.Size(); ++i)
				{
					threads_[i]->Start();
				}
			}

			void ThreadPool::Stop()
			{
				for (Size i = 0; i < threads_.Size(); ++i)
				{
					threads_[i]->Stop();
				}
			};

			void ThreadPool::WaitForStop()
			{
				for (Size i = 0; i < threads_.Size(); ++i)
				{
					threads_[i]->WaitForStop();
				}
			};

			void ThreadPool::WaitForStop(UInt32 wait_time)
			{
				Utility::SystemHelper::Sleep(wait_time);
			}
		}
	}
}