/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once

#include <sstream>
#include <Ultralight/Defines.h>
#include <Ultralight/private/util/DebugBreak.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Logger.h>

#define UL_LOG(level, message) \
  if (ultralight::Logger* logger = ultralight::Platform::instance().logger()) \
    logger->LogMessage(level, message);

#define UL_LOG_ERROR(message) UL_LOG(ultralight::LogLevel::Error, message)
#define UL_LOG_WARNING(message) UL_LOG(ultralight::LogLevel::Warning, message)
#define UL_LOG_INFO(message) UL_LOG(ultralight::LogLevel::Info, message)

#define UL_CHECK_1(condition) {                         \
  if (!(condition)) {                                   \
    std::ostringstream _logstr;                         \
    _logstr << "Check Failed: " << #condition <<        \
      " @ " << __FILE__ << " (" << __LINE__ << ")";     \
    UL_LOG_ERROR(_logstr.str().c_str());                \
    debug_break();                                      \
  }                                                     \
}

#define UL_CHECK_2(condition, message) {                \
  if (!(condition)) {                                   \
    std::ostringstream _logstr;                         \
    _logstr << "Check Failed: " << #condition <<        \
      " @ " << __FILE__ << " (" << __LINE__ << ") " <<  \
       message;                                         \
    UL_LOG_ERROR(_logstr.str().c_str());                \
    debug_break();                                      \
  }                                                     \
}

#define UL_GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define UL_CHECK_CHOOSER(...) \
    UL_GET_3RD_ARG(__VA_ARGS__, UL_CHECK_2, UL_CHECK_1, )
#define UL_CHECK(...) UL_CHECK_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#ifdef NDEBUG
#define UL_DCHECK(...)
#define UL_NOTREACHED()
#define UL_NOTIMPLEMENTED()
#else

#define UL_DCHECK_1(condition) {                        \
  if (!(condition)) {                                   \
    std::ostringstream _logstr;                         \
    _logstr << "Check Failed: " << #condition <<        \
      " @ " << __FILE__ << " (" << __LINE__ << ")";     \
    UL_LOG_ERROR(_logstr.str().c_str());                \
    debug_break();                                      \
  }                                                     \
}

#define UL_DCHECK_2(condition, message) {               \
  if (!(condition)) {                                   \
    std::ostringstream _logstr;                         \
    _logstr << "Check Failed: " << #condition <<        \
      " @ " << __FILE__ << " (" << __LINE__ << ") " <<  \
       message;                                         \
    UL_LOG_ERROR(_logstr.str().c_str());                \
    debug_break();                                      \
  }                                                     \
}

#define UL_DCHECK_CHOOSER(...) \
    UL_GET_3RD_ARG(__VA_ARGS__, UL_DCHECK_2, UL_DCHECK_1, )
#define UL_DCHECK(...) UL_DCHECK_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define UL_NOTREACHED() {                              \
  std::ostringstream _logstr;                          \
  _logstr << "NOTREACHED() Hit: "  <<                  \
    " @ " << __FILE__ << " (" << __LINE__ << ")";      \
  UL_LOG_WARNING(_logstr.str().c_str());               \
  debug_break();                                       \
}

#define UL_NOTIMPLEMENTED() {                          \
  std::ostringstream _logstr;                          \
  _logstr << "NOTIMPLEMENTED() Hit: "  <<              \
    " @ " << __FILE__ << " (" << __LINE__ << ")";      \
  UL_LOG_WARNING(_logstr.str().c_str());               \
  debug_break();                                       \
}

#endif

