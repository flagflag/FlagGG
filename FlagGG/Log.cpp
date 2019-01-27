#include "Log.h"
#include "Define.h"
#include "Allocator/SmartMemory.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace FlagGG
{
    void Log(LogType log_type, const char* format, ...)
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
