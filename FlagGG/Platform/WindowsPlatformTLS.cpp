#if _WIN32
# include "Platform/WindowsPlatformTLS.h"

#include <windows.h>

namespace FlagGG
{

namespace Windows
{

UInt32 TlsAlloc()
{
	return ::TlsAlloc();
}

void TlsSetValue(UInt32 SlotIndex, void* Value)
{
	::TlsSetValue(SlotIndex, Value);
}

void* TlsGetValue(UInt32 SlotIndex)
{
	return ::TlsGetValue(SlotIndex);
}

void TlsFree(UInt32 SlotIndex)
{
	::TlsFree(SlotIndex);
}

UInt32 GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}

}

}
#endif