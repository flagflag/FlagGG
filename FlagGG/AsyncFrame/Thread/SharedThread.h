#ifndef __SHARED_THREAD__
#define __SHARED_THREAD__

#include "AsyncFrame\LockQueue.hpp"
#include "AsyncFrame\Thread\UniqueThread.h"
#include "AsyncFrame\KernelObject\Runtime.h"

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

			class SharedThread : public KernelObject::Runtime
			{
			public:
				SharedThread();

				~SharedThread() override;

				void start();

				void stop() override;

				void waitForStop() override;

				void waitForStop(uint32_t wait_time) override;

				void add(ThreadTask task_func);

				void forceStop();

				uint32_t waitingTime();

			protected:
				void workThread();

			private:
				LockQueue < ThreadTask >	m_task_queue;

				UniqueThreadPtr				m_thread;

				std::atomic < bool >		m_running;
			};

			typedef std::shared_ptr < SharedThread > SharedThreadPtr;
		}
	}
}

#endif