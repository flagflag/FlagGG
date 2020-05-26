#pragma once

#if defined(GameReport_STATIC_DEFINE) || !defined(WIN32)
#  define GameReport_API
#else
#  ifndef GameReport_API
#    ifdef GameReport_EXPORTS
/* We are building this library */
#      define GameReport_API __declspec(dllexport)
#    else
/* We are using this library */
#      define GameReport_API __declspec(dllimport)
#    endif
#  endif
#endif

GameReport_API extern "C" void GameReport_CPUProfilerBegin(const char* pszName);

GameReport_API extern "C" void GameReport_CPUProfilerEnd();

GameReport_API extern "C" void GameReport_CPUProfilerBeginFrame();

GameReport_API extern "C" void GameReport_CPUProfilerEndFrame();
