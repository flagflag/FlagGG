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

				~ThreadPool() override = default;

				void add(ThreadTask task_func);

				void start();

				void stop() override;

				void waitForStop() override;

				void waitForStop(uint32_t wait_time) override;

			private:
				std::vector < SharedThreadPtr > m_threads;
			};
		}
	}
}

#endif