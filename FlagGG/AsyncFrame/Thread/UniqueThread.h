#ifndef __UNIQUE_THREAD__
#define __UNIQUE_THREAD__

#include "AsyncFrame\KernelObject\HandleObject.h"
#include "AsyncFrame\KernelObject\Runtime.h"

#include <functional>
#include <memory>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Thread
		{
			class UniqueThread : public KernelObject::HandleObject, public KernelObject::Runtime
			{
			public:
				UniqueThread(std::function < void(void) > thread_func);

				virtual ~UniqueThread();

				virtual void stop() override;

				virtual void waitForStop() override;

				virtual void waitForStop(DWORD wait_time) override;
			};

			typedef std::shared_ptr < UniqueThread > UniqueThreadPtr;
		}
	}
}

#endif