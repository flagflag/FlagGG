#include "Ultralight/Defines.h"

#ifdef __cplusplus
extern "C" {
#endif

UExport const char* UltralightVersionString()
{
	return ULTRALIGHT_VERSION;
}

UExport uint32_t UltralightVersionMajor()
{
	return ULTRALIGHT_VERSION_MAJOR;
}

UExport uint32_t UltralightVersionMinor()
{
	return ULTRALIGHT_VERSION_MINOR;
}

UExport uint32_t UltralightVersionPatch()
{
	return ULTRALIGHT_VERSION_PATCH;
}

///
/// Full WebKit version string (corresponds to WEBKIT_VERSION)
/// 
UExport const char* WebKitVersionString()
{
	return WEBKIT_VERSION;
}

UExport uint32_t WebKitVersionMajor()
{
	return WEBKIT_VERSION_MAJOR;
}

UExport uint32_t WebKitVersionMinor()
{
	return WEBKIT_VERSION_MINOR;
}

UExport uint32_t WebKitVersionTiny()
{
	return WEBKIT_VERSION_TINY;
}

UExport uint32_t WebKitVersionMicro()
{
	return WEBKIT_VERSION_MICRO;
}

UExport uint32_t WebKitVersionNano()
{
	return WEBKIT_VERSION_NANO;
}

#ifdef __cplusplus
}
#endif