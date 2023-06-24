#pragma once

#if defined(FlagGG_STATIC_DEFINE) || !defined(WIN32)
#  define Editor_API
#else
#  ifndef Editor_API
#    ifdef Editor_EXPORTS
/* We are building this library */
#      define Editor_API __declspec(dllexport)
#    else
/* We are using this library */
#      define Editor_API __declspec(dllimport)
#    endif
#  endif
#endif
