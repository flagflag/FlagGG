#ifndef __HANDLE_OBJECT__
#define __HANDLE_OBJECT__

#include "Export.h"

typedef void* KernelHandle;

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			class FlagGG_API HandleObject
			{
			public:
				HandleObject();

				virtual ~HandleObject();

				KernelHandle getHandle();

			protected:
				KernelHandle m_handle;
			};
		}
	}
}

#endif