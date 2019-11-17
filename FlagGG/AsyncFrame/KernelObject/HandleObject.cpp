#include "HandleObject.h"

#if _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			HandleObject::HandleObject()
				: handle_(NULL_HANDLE)
			{ }

			HandleObject::~HandleObject()
			{
				if (handle_)
				{
#if _WIN32
					CloseHandle((HANDLE)handle_);
#endif
					handle_ = NULL_HANDLE;
				}
			}

			KernelHandle HandleObject::GetHandler()
			{
				return handle_;
			}
		}
	}
}