#ifndef __RUNTIME__
#define __RUNTIME__

#include <windows.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			class Runtime
			{
			public:
				virtual void stop() = 0;

				virtual void waitForStop() = 0;

				virtual void waitForStop(DWORD) = 0;
			};
		}
	}
}

#endif