#ifndef __HANDLE_OBJECT__
#define __HANDLE_OBJECT__

#include "Export.h"

typedef void* KernelHandle;
#define NULL_HANDLE nullptr

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

				KernelHandle GetHandle();

			protected:
				KernelHandle handle_;
			};
		}
	}
}

#endif