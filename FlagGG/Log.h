#ifndef __FLAGGG_LOG__
#define __FLAGGG_LOG__

#include "Export.h"
#include "Container/Str.h"
#include "AsyncFrame/Mutex.h"
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

	class FlagGG_API Logger
	{
	public:
		Logger();

		virtual ~Logger();

		std::shared_ptr<spdlog::logger> Default();

		void AddLogger(const Container::String& name, const Container::String& path);

		void RemoveLogger(const Container::String& name);

		std::shared_ptr<spdlog::logger> GetLogger(const Container::String& name);

		static Logger* GetInstance();

		static void DestroyInstance();

	private:
		static volatile bool initialized_;
		static Logger* logger_;

		static AsyncFrame::Mutex mutex_;
	};
}

#define FLAGGG_LOG_DEBUG(format, ...) FlagGG::Logger::GetInstance()->Default()->debug(format, ##__VA_ARGS__)

#define FLAGGG_LOG_INFO(format, ...) FlagGG::Logger::GetInstance()->Default()->info(format, ##__VA_ARGS__)

#define FLAGGG_LOG_WARN(format, ...) FlagGG::Logger::GetInstance()->Default()->warn(format, ##__VA_ARGS__)

#define FLAGGG_LOG_ERROR(format, ...) FlagGG::Logger::GetInstance()->Default()->error(format, ##__VA_ARGS__)

#define FLAGGG_LOG_CRITICAL(format, ...) FlagGG::Logger::GetInstance()->Default()->critical(format, ##__VA_ARGS__)

#endif
