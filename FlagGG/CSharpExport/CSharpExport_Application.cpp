#include "CSharpExport.h"

CSharp_API bool Application_IsPlaying()
{
	return false;
}

CSharp_API int Screen_GetWidth()
{
	return 1600;
}

CSharp_API int Screen_GetHeight()
{
	return 900;
}

CSharp_API float Screen_GetDPI()
{
	return 96.0f;
}
