#pragma once

#if defined(FlagGG_STATIC_DEFINE) || !defined(WIN32)
#  define Game_API
#else
#  ifndef Game_API
#    ifdef Game_EXPORTS
/* We are building this library */
#      define Game_API __declspec(dllexport)
#    else
/* We are using this library */
#      define Game_API __declspec(dllimport)
#    endif
#  endif
#endif

Game_API int EntryPoint(int argc, const char* argv[]);
