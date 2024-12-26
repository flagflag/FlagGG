#pragma once

#if defined(FlagGG_STATIC_DEFINE) || !defined(WIN32)
#  define FlagGG_API
#else
#  ifndef FlagGG_API
#    ifdef FlagGG_EXPORTS
/* We are building this library */
#      define FlagGG_API __declspec(dllexport)
#    else
/* We are using this library */
#      define FlagGG_API __declspec(dllimport)
#    endif
#  endif
#endif
