#pragma once

#if defined(FlagGG_STATIC_DEFINE) || !defined(WIN32)
#  define CoreCLRHelper_API
#else
#  ifndef CoreCLRHelper_API
#    ifdef CoreCLRHelper_EXPORTS
/* We are building this library */
#      define CoreCLRHelper_API __declspec(dllexport)
#    else
/* We are using this library */
#      define CoreCLRHelper_API __declspec(dllimport)
#    endif
#  endif
#endif
