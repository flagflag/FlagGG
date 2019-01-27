#include "UniqueThread.h"
#include "Log.h"

#if WIN32 || WIN64
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
#if !WIN32 && !WIN64
				pthread_cond_t* pcond;
#endif
			};

			static THREAD_RETURN THREAD_MARK ThreadFunc(THREAD_PARAM inParam)
			{
				ThreadParam* param = (ThreadParam*)inParam;
				
				if (param && param->thread_func)
				{
					param->thread_func();
#if !WIN32 && !WIN64
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
#if WIN32 || WIN64
					m_handle = CreateThread(nullptr, 0, ThreadFunc, param, 0, nullptr);

					if (!m_handle)
					{
						FLAGGG_LOG_ERROR("create thread failed!");
					}
#else
					pthread_mutex_init(&m_mutex, nullptr);
					pthread_cond_init(&m_cond, nullptr);
					param->pcond = &m_cond;
					pthread_t thread_id;
					if (0 == pthread_create((pthread_t*)m_handle, nullptr, ThreadFunc, param))
					{
						m_handle = &thread_id;
					}
					else
					{
						FLAGGG_LOG_ERROR("create thread failed!");
					}
#endif
				}
			}

			UniqueThread::~UniqueThread()
			{
#if !WIN32 && !WIN64
				pthread_mutex_destroy(&m_mutex);
				pthread_cond_destroy(&m_cond);
#endif
			}

			void UniqueThread::stop()
			{
				
#if WIN32 || WIN64
				TerminateThread(m_handle, -1);
#else
				if (!m_handle)
				{
					pthread_cancel(*((pthread_t*)m_handle));
				}
#endif
			}

			void UniqueThread::waitForStop()
			{
#if WIN32 || WIN64
				WaitForSingleObject(m_handle, INFINITE);
#else
				if(!m_handle)
				{
					pthread_join(*((pthread_t*)m_handle), nullptr);
				}
#endif
			}

			void UniqueThread::waitForStop(uint32_t wait_time)
			{
#if WIN32 || WIN64
				WaitForSingleObject(m_handle, wait_time);
#else
				pthread_mutex_lock(&m_mutex);
				static timespec out_time;
				out_time.tv_sec = 0;
				out_time.tv_nsec = wait_time * 1000;
				pthread_cond_timedwait(&m_cond, &m_mutex, &out_time);
				pthread_mutex_unlock(&m_mutex);
#endif
			}
		}
	}
}