#ifndef __UNIQUE_THREAD__
#define __UNIQUE_THREAD__

#include "AsyncFrame/KernelObject/HandleObject.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Export.h"

#include <functional>

#ifndef _WIN32
#include <pthread.h>
#endif

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			class FlagGG_API UniqueThread : public KernelObject::HandleObject, public KernelObject::Runtime, public Container::RefCounted
			{
			public:
				UniqueThread(std::function < void(void) > thread_func);

				~UniqueThread() override;

				void Stop() override;

				void WaitForStop() override;

				void WaitForStop(UInt32 wait_time) override;

#ifndef _WIN32
				pthread_mutex_t mutex_;
				pthread_cond_t cond_;
#endif
			};

			typedef Container::SharedPtr < UniqueThread > UniqueThreadPtr;
		}
	}
}

#endif