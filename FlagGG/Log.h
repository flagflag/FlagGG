#ifndef __FLAGGG_LOG__
#define __FLAGGG_LOG__

#include "Export.h"

namespace FlagGG
{
    enum LogType
    {
        LOG_DEBUG = 0,
        LOG_INFO,
		LOG_WARN,
        LOG_ERROR,
		LOG_CRITICAL
    };

    void FlagGG_API Log(LogType log_type, const char* format, ...);
}

#define FLAGGG_LOG_DEBUG(format, ...) FlagGG::Log(FlagGG::LOG_DEBUG, format, ##__VA_ARGS__)

#define FLAGGG_LOG_INFO(format, ...) FlagGG::Log(FlagGG::LOG_INFO, format, ##__VA_ARGS__)

#define FLAGGG_LOG_WARN(format, ...) FlagGG::Log(FlagGG::LOG_WARN, format, ##__VA_ARGS__)

#define FLAGGG_LOG_ERROR(format, ...) FlagGG::Log(FlagGG::LOG_ERROR, format, ##__VA_ARGS__)

#define FLAGG_LOG_CRITICAL(format, ...) FlagGG::Log(FlagGG::LOG_CRITICAL, format, ##__VA_ARGS__)

#endif
