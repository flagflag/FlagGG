#ifndef __RUNTIME__
#define __RUNTIME__

#include "Export.h"

#include <stdint.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			class FlagGG_API Runtime
			{
			public:
				virtual ~Runtime() = default;

				virtual void stop() = 0;

				virtual void waitForStop() = 0;

				virtual void waitForStop(uint32_t) = 0;
			};
		}
	}
}

#endif