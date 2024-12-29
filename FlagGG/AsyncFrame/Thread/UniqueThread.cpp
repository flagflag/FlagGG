#include "UniqueThread.h"
#include "Log.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#define THREAD_MARK WINAPI
#define THREAD_RETURN DWORD
#define THREAD_PARAM LPVOID
#else
#define THREAD_MARK
#define THREAD_RETURN void*
#define THREAD_PARAM void*
#endif

namespace FlagGG
{

struct ThreadParam
{
	IRunnable* runnable_;
#if !PLATFORM_WINDOWS
	pthread_cond_t* pcond_;
#endif
};

static THREAD_RETURN THREAD_MARK ThreadFunc(THREAD_PARAM inParam)
{
	ThreadParam* param = (ThreadParam*)inParam;
	if (param && param->runnable_)
	{
		param->runnable_->Run();
#if !PLATFORM_WINDOWS
		pthread_cond_signal(param->pcond_);
#endif
		param->runnable_->Exit();
		delete param;
	}

	return 0;
}

class ThreadRunnable : public IRunnable
{
public:
	ThreadRunnable(const std::function<void(void)>& threadFunc)
		: threadFunc_(threadFunc)
	{
		if (!threadFunc_)
			throw "Empty thread function.";
	}

	ThreadRunnable(std::function<void(void)>&& threadFunc)
		: threadFunc_(std::move(threadFunc))
	{
		if (!threadFunc_)
			throw "Empty thread function.";
	}

	void Init() override
	{

	}

	void Run() override
	{
		threadFunc_();
	}

	void Stop() override
	{

	}

	void Exit() override
	{

	}

private:
	std::function<void(void)> threadFunc_;
};

UniqueThread::UniqueThread(const std::function<void(void)>& threadFunc)
{
	runnable_ = new ThreadRunnable(threadFunc);
	CreateRunnableThread(runnable_);
}

UniqueThread::UniqueThread(std::function<void(void)>&& threadFunc)
{
	runnable_ = new ThreadRunnable(std::move(threadFunc));
	CreateRunnableThread(runnable_);
}

UniqueThread::UniqueThread(IRunnable* runnable)
	: runnable_(runnable)
{
	if (runnable_)
	{
		CreateRunnableThread(runnable_);
	}
}

UniqueThread::~UniqueThread()
{
#if !PLATFORM_WINDOWS
	pthread_mutex_destroy(&mutex_);
	pthread_cond_destroy(&cond_);
	if (handle_)
	{
		delete handle_;
		handle_ = nullptr;
	}
#endif
}

void UniqueThread::CreateRunnableThread(IRunnable* runnable)
{
	ThreadParam* param = new ThreadParam();
	param->runnable_ = runnable;
#if PLATFORM_WINDOWS
	handle_ = CreateThread(nullptr, 0, ThreadFunc, param, 0, nullptr);

	if (!handle_)
	{
		FLAGGG_LOG_ERROR("create thread failed!");
	}
#else
	pthread_mutex_init(&mutex_, nullptr);
	pthread_cond_init(&cond_, nullptr);
	param->pcond_ = &cond_;
	pthread_t* thread_id = new pthread_t();
	if (0 == pthread_create(thread_id, nullptr, ThreadFunc, param))
	{
		handle_ = thread_id;
	}
	else
	{
		delete thread_id;
		FLAGGG_LOG_ERROR("create thread failed!");
	}
#endif
}

void UniqueThread::Stop()
{
				
#if PLATFORM_WINDOWS
	TerminateThread(handle_, -1);
#else
	if (handle_)
	{
		pthread_cancel(*((pthread_t*)handle_));
	}
#endif
}

void UniqueThread::WaitForStop()
{
#if PLATFORM_WINDOWS
	WaitForSingleObject(handle_, INFINITE);
#else
	if (handle_)
	{
		pthread_join(*((pthread_t*)handle_), nullptr);
	}
#endif
}

void UniqueThread::WaitForStop(UInt32 waitTime)
{
#if PLATFORM_WINDOWS
	WaitForSingleObject(handle_, waitTime);
#else
	pthread_mutex_lock(&mutex_);
	static timespec out_time;
	out_time.tv_sec = 0;
	out_time.tv_nsec = waitTime * 1000;
	pthread_cond_timedwait(&cond_, &mutex_, &out_time);
	pthread_mutex_unlock(&mutex_);
#endif
}

UInt64 UniqueThread::GetCurrentThreadId()
{
#if PLATFORM_WINDOWS
	return ::GetCurrentThreadId();
#else
	return 0;
#endif
}

}