#include "Log.h"
#include "Define.h"
#include "Allocator/SmartMemory.hpp"
#include "Utility/SystemHelper.h"

#include <stdarg.h>
#include <stdio.h>

namespace FlagGG
{
	static const std::string FLAGGG_LOG = "FlagGGLog";

	Logger::Logger()
	{
		try
		{
			Container::String logDir = Utility::SystemHelper::GetProgramDir() + "logs";
			if (!Utility::SystemHelper::DirExists(logDir))
				Utility::SystemHelper::CreateDir(logDir);
			Container::String logPath = logDir + "/FlagGGLog-" + Utility::SystemHelper::GetTimeStamp() + ".log";
			auto FlagGGLog = spdlog::basic_logger_mt(FLAGGG_LOG, logPath.CString());
			FlagGGLog->flush_on(spdlog::level::debug);
			spdlog::register_logger(FlagGGLog);

		}
		catch (spdlog::spdlog_ex& ex)
		{
			Log(LOG_ERROR, "There is something wrong with spdlog[%s].", ex.what());
		}
	}

	Logger::~Logger()
	{
	}

	std::shared_ptr<spdlog::logger> Logger::Default()
	{
		return spdlog::get(FLAGGG_LOG);
	}

	void Logger::AddLogger(const Container::String& name, const Container::String& path)
	{
		spdlog::basic_logger_mt(name.CString(), path.CString());
	}

	void Logger::RemoveLogger(const Container::String& name)
	{
		spdlog::drop(name.CString());
	}

	std::shared_ptr<spdlog::logger> Logger::GetLogger(const Container::String& name)
	{
		return spdlog::get(name.CString());
	}

	Logger* Logger::GetInstance()
	{
		if (!initialized_)
		{
			mutex_.Lock();

			if (!initialized_)
			{
				logger_ = new Logger();
				initialized_ = true;
			}

			mutex_.UnLock();
		}

		return logger_;
	}

	void Logger::DestroyInstance()
	{
		if (initialized_)
		{
			mutex_.Lock();

			if (initialized_)
			{
				delete logger_;
				logger_ = nullptr;
				initialized_ = false;
			}

			mutex_.UnLock();
		}
	}

	volatile bool Logger::initialized_ = false;
	Logger* Logger::logger_ = nullptr;
	AsyncFrame::Mutex Logger::mutex_;

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
        
        char _buffer[ONE_KB + 1];
        Allocator::SmartMemory <char> temp(len + 1, len <= ONE_KB ? _buffer : nullptr);
        char* buffer = temp.Get();

        vsnprintf(buffer, len + 1, format, va);
        va_end(va);
		

		buffer[len] = '\0';
		puts(buffer);
    }
}
