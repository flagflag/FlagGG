#ifndef __UNIQUE_THREAD__
#define __UNIQUE_THREAD__

#include "AsyncFrame/KernelObject/HandleObject.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "Export.h"

#include <functional>
#include <memory>

#if !WIN32 && !WIN64
#include <pthread.h>
#endif

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			class FlagGG_API UniqueThread : public KernelObject::HandleObject, public KernelObject::Runtime
			{
			public:
				UniqueThread(std::function < void(void) > thread_func);

				~UniqueThread() override;

				void stop() override;

				void waitForStop() override;

				void waitForStop(uint32_t wait_time) override;

#if !WIN32 && !WIN64
				pthread_mutex_t m_mutex;
				pthread_cond_t m_cond;
#endif
			};

			typedef std::shared_ptr < UniqueThread > UniqueThreadPtr;
		}
	}
}

#endif