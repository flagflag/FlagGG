#ifndef __HANDLE_OBJECT__
#define __HANDLE_OBJECT__

#include <windows.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			class HandleObject
			{
			public:
				HandleObject();

				virtual ~HandleObject();

				HANDLE getHandle();

			protected:
				HANDLE m_handle;
			};
		}
	}
}

#endif