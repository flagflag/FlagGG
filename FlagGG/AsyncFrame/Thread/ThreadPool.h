#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "Export.h"

#include "SharedThread.h"

#include <vector>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			class FlagGG_API ThreadPool : public KernelObject::Runtime
			{
			public:
				ThreadPool(size_t thread_count);

				~ThreadPool() override = default;

				void Add(ThreadTask task_func);

				void Start();

				void Stop() override;

				void WaitForStop() override;

				void WaitForStop(uint32_t wait_time) override;

			private:
				std::vector < SharedThreadPtr > threads_;
			};
		}
	}
}

#endif