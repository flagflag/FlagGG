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

				virtual void Stop() = 0;

				virtual void WaitForStop() = 0;

				virtual void WaitForStop(uint32_t) = 0;
			};
		}
	}
}

#endif