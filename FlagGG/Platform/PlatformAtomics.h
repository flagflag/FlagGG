//
// 平台原子操作接口
//

#pragma once

#include "Core/GenericPlatform.h"
#ifdef PLATFORM_WINDOWS
#include "Platform/WindowsPlatformAtomics.h"
#else
#error
#endif
