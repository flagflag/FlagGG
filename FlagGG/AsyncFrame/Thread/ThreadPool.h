#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "Export.h"

#include "SharedThread.h"
#include "Container/Vector.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			class FlagGG_API ThreadPool : public KernelObject::Runtime, public Container::RefCounted
			{
			public:
				ThreadPool(UInt32 threadCount);

				~ThreadPool() override = default;

				void Add(ThreadTask task_func);

				void Start();

				void Stop() override;

				void WaitForStop() override;

				void WaitForStop(UInt32 wait_time) override;

			private:
				Container::Vector<SharedThreadPtr> threads_;
			};
		}
	}
}

#endif