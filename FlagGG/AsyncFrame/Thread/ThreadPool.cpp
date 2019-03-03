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

			ThreadPool::ThreadPool(size_t thread_count)
			{
				assert(thread_count <= THREAD_POOL_MAX_SIZE);

				for (size_t i = 0; i < thread_count; ++i)
				{
					m_threads.emplace_back(new SharedThread);
				}
			}

			void ThreadPool::Add(ThreadTask task_func)
			{
				size_t index = 0;
				uint32_t min_waiting_time = INT_MAX;

				for (size_t i = 0; i < m_threads.size(); ++i)
				{
					uint32_t waiting_time = m_threads[i]->WaitingTime();
					if (waiting_time < min_waiting_time)
					{
						index = i;
						min_waiting_time = waiting_time;
					}
				}

				if (m_threads.size() > 0)
				{
					m_threads[index]->Add(task_func);
				}
			}

			void ThreadPool::Start()
			{
				for (size_t i = 0; i < m_threads.size(); ++i)
				{
					m_threads[i]->Start();
				}
			}

			void ThreadPool::Stop()
			{
				for (size_t i = 0; i < m_threads.size(); ++i)
				{
					m_threads[i]->Stop();
				}
			};

			void ThreadPool::WaitForStop()
			{
				for (size_t i = 0; i < m_threads.size(); ++i)
				{
					m_threads[i]->WaitForStop();
				}
			};

			void ThreadPool::WaitForStop(uint32_t wait_time)
			{
				Utility::SystemHelper::Sleep(wait_time);
			}
		}
	}
}