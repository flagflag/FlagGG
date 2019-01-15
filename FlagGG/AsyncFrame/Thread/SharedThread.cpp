#include "SharedThread.h"

#include <windows.h>

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
				stop();
				waitForStop();
			}

			void SharedThread::start()
			{
				if (!m_running)
				{
					m_running = true;

					m_thread.reset(new UniqueThread(std::bind(&SharedThread::workThread, this)));
				}		
			}

			void SharedThread::stop()
			{
				if (m_running && m_thread)
				{
					m_running = false;
				}
			}

			void SharedThread::forceStop()
			{
				if (m_running && m_thread)
				{
					m_running = false;

					m_thread->stop();
				}
			}

			void SharedThread::waitForStop()
			{
				if (m_running && m_thread)
				{
					m_thread->waitForStop();
				}
			}

			void SharedThread::waitForStop(uint32_t wait_time)
			{
				if (m_running && m_thread)
				{
					m_thread->waitForStop(wait_time);
				}
			}

			void SharedThread::add(ThreadTask task_func)
			{
				if (m_running && m_thread)
				{
					m_task_queue.push(task_func);
				}
			}

			uint32_t SharedThread::waitingTime()
			{
				return (uint32_t)m_task_queue.size();
			}

			void SharedThread::workThread()
			{
				while (m_running)
				{
					Sleep(16);

					if (m_task_queue.size() > 0)
					{
						LockQueue < ThreadTask >::Objects objects;
						m_task_queue.slipce(objects);

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