#ifndef __SHARED_THREAD__
#define __SHARED_THREAD__

#include "Export.h"

#include "AsyncFrame/ConditionQueue.hpp"
#include "AsyncFrame/Thread/UniqueThread.h"
#include "AsyncFrame/KernelObject/Runtime.h"

#include <functional>
#include <atomic>
#include <stdint.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			typedef std::function < void(void) > ThreadTask;

			class FlagGG_API SharedThread : public KernelObject::Runtime, public Container::RefCounted
			{
			public:
				SharedThread();

				~SharedThread() override;

				void Start();

				void Stop() override;

				void WaitForStop() override;

				void WaitForStop(uint32_t wait_time) override;

				void Add(ThreadTask task_func);

				void ForceStop();

				uint32_t WaitingTime();

			protected:
				void WorkThread();

			private:
				ConditionQueue<ThreadTask>	taskQueue_;

				UniqueThreadPtr				thread_;

				std::atomic<bool>			running_;
			};

			typedef Container::SharedPtr < SharedThread > SharedThreadPtr;
		}
	}
}

#endif