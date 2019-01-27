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

    void FlagGG_API Log(LogType log_type, const char* format, ...)
    {
        if (log_type == LOG_DEBUG)
        {
#if !DEBUG && !_DEBUG
            return;
#endif
        }

        va_list va;
        va_start(va, format);
        size_t len = vsnprintf(nullptr, 0, format, va);
        
        char _buffer[ONE_KB];
        Allocator::SmartMemory <char> temp(len, len <= ONE_KB ? _buffer : nullptr);
        char* buffer = temp.get();

        vsnprintf(buffer, len, format, va);
        va_end(va);

        puts(buffer);
    }
}

#define FLAGGG_LOG_DEBUG(format, ...) FlagGG::Log(FlagGG::LOG_DEBUG, format, ##__VA_ARGS__)

#define FLAGGG_LOG_INFO(format, ...) FlagGG::Log(FlagGG::LOG_INFO, format, ##__VA_ARGS__)

#define FLAGGG_LOG_ERROR(format, ...) FlagGG::Log(FlagGG::LOG_ERROR, format, ##__VA_ARGS__)

#endif
