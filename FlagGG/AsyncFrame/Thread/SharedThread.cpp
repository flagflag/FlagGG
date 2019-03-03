#include "SharedThread.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			SharedThread::SharedThread()
				: m_thread(nullptr)
				, m_running(false)
			{ }

			SharedThread::~SharedThread()
			{
				Stop();
				WaitForStop();
			}

			void SharedThread::Start()
			{
				if (!m_running)
				{
					m_running = true;

					m_thread.reset(new UniqueThread(std::bind(&SharedThread::WorkThread, this)));
				}		
			}

			void SharedThread::Stop()
			{
				if (m_running && m_thread)
				{
					m_running = false;
				}
			}

			void SharedThread::ForceStop()
			{
				if (m_running && m_thread)
				{
					m_running = false;

					m_thread->Stop();
				}
			}

			void SharedThread::WaitForStop()
			{
				if (m_running && m_thread)
				{
					m_thread->WaitForStop();
				}
			}

			void SharedThread::WaitForStop(uint32_t wait_time)
			{
				if (m_running && m_thread)
				{
					m_thread->WaitForStop(wait_time);
				}
			}

			void SharedThread::Add(ThreadTask task_func)
			{
				if (m_running && m_thread)
				{
					m_task_queue.Push(task_func);
				}
			}

			uint32_t SharedThread::WaitingTime()
			{
				return (uint32_t)m_task_queue.Size();
			}

			void SharedThread::WorkThread()
			{
				while (m_running)
				{
					Utility::SystemHelper::Sleep(16);

					if (m_task_queue.Size() > 0)
					{
						LockQueue < ThreadTask >::Objects objects;
						m_task_queue.Slipce(objects);

						for (auto it = objects.begin(); it != objects.end() && m_running; ++it)
						{
							if ((*it))
							{
								(*it)();
							}
						}
					}
				}
			}
		}
	}
}