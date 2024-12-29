#include "HandleObject.h"
#include "Core/GenericPlatform.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace FlagGG
{

HandleObject::HandleObject()
	: handle_(NULL_HANDLE)
{ }

HandleObject::~HandleObject()
{
	if (handle_)
	{
#if PLATFORM_WINDOWS
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