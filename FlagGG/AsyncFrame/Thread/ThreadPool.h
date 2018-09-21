#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "SharedThread.h"

#include <vector>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			class ThreadPool : public KernelObject::Runtime
			{
			public:
				ThreadPool(size_t thread_count);

				virtual ~ThreadPool();

				void add(ThreadTask task_func);

				void start();

				virtual void stop() override;

				virtual void waitForStop() override;

				virtual void waitForStop(DWORD wait_time) override;

			private:
				std::vector < SharedThreadPtr > m_threads;
			};
		}
	}
}

#endif