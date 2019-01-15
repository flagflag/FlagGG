#ifndef __UNIQUE_THREAD__
#define __UNIQUE_THREAD__

#include "AsyncFrame\KernelObject\HandleObject.h"
#include "AsyncFrame\KernelObject\Runtime.h"
#include "Export.h"

#include <functional>
#include <memory>

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

				~UniqueThread() override = default;

				void stop() override;

				void waitForStop() override;

				void waitForStop(uint32_t wait_time) override;
			};

			typedef std::shared_ptr < UniqueThread > UniqueThreadPtr;
		}
	}
}

#endif