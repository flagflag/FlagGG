#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Core/Subsystem.h"
#include "AsyncFrame/Mutex.h"
#include "Utility/Format.h"
#include "spdlog/spdlog.h"

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

class FlagGG_API Logger : public Subsystem<Logger>
{
public:
	Logger();

	virtual ~Logger();

	std::shared_ptr<spdlog::logger> Default();

	void AddLogger(const String& name, const String& path);

	void RemoveLogger(const String& name);

	std::shared_ptr<spdlog::logger> GetLogger(const String& name);
};

}

#define FLAGGG_LOG_DEBUG(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->debug(format, ##__VA_ARGS__)

#define FLAGGG_LOG_INFO(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->info(format, ##__VA_ARGS__)

#define FLAGGG_LOG_WARN(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->warn(format, ##__VA_ARGS__)

#define FLAGGG_LOG_ERROR(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->error(format, ##__VA_ARGS__)

#define FLAGGG_LOG_CRITICAL(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->critical(format, ##__VA_ARGS__)


#define FLAGGG_LOG_STD_DEBUG(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->debug(FlagGG::ToString(format, ##__VA_ARGS__).CString())

#define FLAGGG_LOG_STD_INFO(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->info(FlagGG::ToString(format, ##__VA_ARGS__).CString())

#define FLAGGG_LOG_STD_WARN(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->warn(FlagGG::ToString(format, ##__VA_ARGS__).CString())

#define FLAGGG_LOG_STD_ERROR(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->error(FlagGG::ToString(format, ##__VA_ARGS__).CString())

#define FLAGGG_LOG_STD_CRITICAL(format, ...) FlagGG::GetSubsystem<FlagGG::Logger>()->Default()->critical(FlagGG::ToString(format, ##__VA_ARGS__).CString())
