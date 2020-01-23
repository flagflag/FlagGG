#ifndef __SHARED_THREAD__
#define __SHARED_THREAD__

#include "Export.h"

#include "AsyncFrame/ConditionQueue.hpp"
#include "AsyncFrame/Thread/UniqueThread.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "Core/BaseTypes.h"

#include <functional>
#include <atomic>

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

				void WaitForStop(UInt32 wait_time) override;

				void Add(ThreadTask task_func);

				void ForceStop();

				UInt32 WaitingTime();

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