#ifndef __FLAGGG_LOG__
#define __FLAGGG_LOG__

#include "Export.h"

#include "Define.h"
#include "Allocator/SmartMemory.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace FlagGG
{
    enum LogType
    {
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_ERROR
    };

    void FlagGG_API Log(LogType log_type, const char* format, ...);
}

#define FLAGGG_LOG_DEBUG(format, ...) FlagGG::Log(FlagGG::LOG_DEBUG, format, ##__VA_ARGS__)

#define FLAGGG_LOG_INFO(format, ...) FlagGG::Log(FlagGG::LOG_INFO, format, ##__VA_ARGS__)

#define FLAGGG_LOG_ERROR(format, ...) FlagGG::Log(FlagGG::LOG_ERROR, format, ##__VA_ARGS__)

#endif
