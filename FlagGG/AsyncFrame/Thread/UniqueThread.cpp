#include "UniqueThread.h"
#include "Log.h"

#if _WIN32
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
	namespace AsyncFrame
	{
		namespace Thread
		{
			struct ThreadParam
			{
				std::function < void(void) > thread_func;
#ifndef _WIN32
				pthread_cond_t* pcond;
#endif
			};

			static THREAD_RETURN THREAD_MARK ThreadFunc(THREAD_PARAM inParam)
			{
				ThreadParam* param = (ThreadParam*)inParam;
				if (param && param->thread_func)
				{
					param->thread_func();
#ifndef _WIN32
					pthread_cond_signal(param->pcond);
#endif
					delete param;
				}

				return 0;
			}

			UniqueThread::UniqueThread(std::function < void(void) > thread_func)
			{
				if (thread_func)
				{
					ThreadParam* param = new ThreadParam();
					param->thread_func = thread_func;
#if _WIN32
					handle_ = CreateThread(nullptr, 0, ThreadFunc, param, 0, nullptr);

					if (!handle_)
					{
						FLAGGG_LOG_ERROR("create thread failed!");
					}
#else
					pthread_mutex_init(&mutex_, nullptr);
					pthread_cond_init(&cond_, nullptr);
					param->pcond = &cond_;
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
			}

			UniqueThread::~UniqueThread()
			{
#ifndef _WIN32
				pthread_mutex_destroy(&mutex_);
				pthread_cond_destroy(&cond_);
				if (handle_)
				{
					delete handle_;
					handle_ = nullptr;
				}
#endif
			}

			void UniqueThread::Stop()
			{
				
#if _WIN32
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
#if _WIN32
				WaitForSingleObject(handle_, INFINITE);
#else
				if (handle_)
				{
					pthread_join(*((pthread_t*)handle_), nullptr);
				}
#endif
			}

			void UniqueThread::WaitForStop(UInt32 wait_time)
			{
#if _WIN32
				WaitForSingleObject(handle_, wait_time);
#else
				pthread_mutex_lock(&mutex_);
				static timespec out_time;
				out_time.tv_sec = 0;
				out_time.tv_nsec = wait_time * 1000;
				pthread_cond_timedwait(&cond_, &mutex_, &out_time);
				pthread_mutex_unlock(&mutex_);
#endif
			}
		}
	}
}