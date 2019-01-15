#include "UniqueThread.h"

#include <windows.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			struct ThreadParam
			{
				std::function < void(void) > thread_func;
			};

			static DWORD WINAPI ThreadFunc(LPVOID lpParam)
			{
				ThreadParam* param = (ThreadParam*)lpParam;
				
				if (param && param->thread_func)
				{
					param->thread_func();

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

					m_handle = CreateThread(nullptr, 0, ThreadFunc, param, 0, nullptr);
				}
			}

			void UniqueThread::stop()
			{
				TerminateThread(m_handle, -1);
			}

			void UniqueThread::waitForStop()
			{
				WaitForSingleObject(m_handle, INFINITE);
			}

			void UniqueThread::waitForStop(uint32_t wait_time)
			{
				WaitForSingleObject(m_handle, wait_time);
			}
		}
	}
}