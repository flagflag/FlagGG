#include "SharedThread.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			SharedThread::SharedThread()
				: thread_(nullptr)
				, running_(false)
			{ }

			SharedThread::~SharedThread()
			{
				Stop();
				WaitForStop();
			}

			void SharedThread::Start()
			{
				if (!running_)
				{
					running_ = true;

					thread_.reset(new UniqueThread(std::bind(&SharedThread::WorkThread, this)));
				}		
			}

			void SharedThread::Stop()
			{
				if (running_ && thread_)
				{
					running_ = false;
				}
			}

			void SharedThread::ForceStop()
			{
				if (running_ && thread_)
				{
					running_ = false;

					thread_->Stop();
				}
			}

			void SharedThread::WaitForStop()
			{
				if (running_ && thread_)
				{
					thread_->WaitForStop();
				}
			}

			void SharedThread::WaitForStop(uint32_t wait_time)
			{
				if (running_ && thread_)
				{
					thread_->WaitForStop(wait_time);
				}
			}

			void SharedThread::Add(ThreadTask task_func)
			{
				if (running_ && thread_)
				{
					task_queue_.Push(task_func);
				}
			}

			uint32_t SharedThread::WaitingTime()
			{
				return (uint32_t)task_queue_.Size();
			}

			void SharedThread::WorkThread()
			{
				while (running_)
				{
					Utility::SystemHelper::Sleep(16);

					if (task_queue_.Size() > 0)
					{
						LockQueue < ThreadTask >::Objects objects;
						task_queue_.Slipce(objects);

						for (auto it = objects.begin(); it != objects.end() && running_; ++it)
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