//
// 平台TLS
//

#pragma once

#include "Core/GenericPlatform.h"
#ifdef PLATFORM_WINDOWS
#include "Platform/WindowsPlatformTLS.h"
#elif __APPLE__
#include "Platform/ApplePlatformTLS.h"
#else
#error
#endif
