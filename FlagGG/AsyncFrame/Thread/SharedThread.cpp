#include "SharedThread.h"
#include "Utility/SystemHelper.h"
#include "Log.h"

namespace FlagGG
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
		taskQueue_.Release();
	}
}

void SharedThread::ForceStop()
{
	if (running_ && thread_)
	{
		running_ = false;
		taskQueue_.Release();

		thread_->Stop();
	}
}

void SharedThread::WaitForStop()
{
	if (thread_)
	{
		thread_->WaitForStop();
	}
}

void SharedThread::WaitForStop(UInt32 wait_time)
{
	if (thread_)
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

UInt32 SharedThread::WaitingTime()
{
	return (UInt32)taskQueue_.Size();
}

#define CHECK_EXIT() if (!running_) break;

void SharedThread::WorkThread()
{
	FLAGGG_LOG_INFO("Shared thread start.");

	ConditionQueue<ThreadTask>::Objects objects;

	while (running_)
	{
		taskQueue_.Swap(objects);

		CHECK_EXIT();

		for (auto it = objects.Begin(); it != objects.End(); ++it)
		{
			CHECK_EXIT();

			if ((*it))
				(*it)();
		}

		objects.Clear();

		CHECK_EXIT();
	}

	FLAGGG_LOG_INFO("Shared thread stop.");
}

}