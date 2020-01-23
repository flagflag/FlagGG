#ifndef __RUNTIME__
#define __RUNTIME__

#include "Export.h"

#include "Core/BaseTypes.h"

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

				virtual void WaitForStop(UInt32) = 0;
			};
		}
	}
}

#endif