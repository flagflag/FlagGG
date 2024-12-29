//
// 平台TLS
//

#pragma once

#include "Core/GenericPlatform.h"
#ifdef PLATFORM_WINDOWS
#include "Platform/WindowsPlatformTLS.h"
#else
#error
#endif
