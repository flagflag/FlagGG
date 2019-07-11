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

					thread_ = new UniqueThread(std::bind(&SharedThread::WorkThread, this));
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
					taskQueue_.PushBack(task_func);
				}
			}

			uint32_t SharedThread::WaitingTime()
			{
				return (uint32_t)taskQueue_.Size();
			}

#define CHECK_EXIT() if (!running_) return;

			void SharedThread::WorkThread()
			{
				while (running_)
				{
					taskQueue_.Wait();

					CHECK_EXIT();

					while (!taskQueue_.IsEmpty())
					{
						auto& objects = taskQueue_.Swap();

						CHECK_EXIT();

						for (auto it = objects.Begin(); it != objects.End(); ++it)
						{
							CHECK_EXIT();

							if ((*it))
							{
								(*it)();
							}
						}
					}

					CHECK_EXIT();
				}
			}
		}
	}
}