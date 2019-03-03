#include "HandleObject.h"

#if WIN32 || WIN64
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
#if WIN32 || WIN64
					CloseHandle((HANDLE)handle_);
#endif
					handle_ = NULL_HANDLE;
				}
			}

			KernelHandle HandleObject::GetHandle()
			{
				return handle_;
			}
		}
	}
}