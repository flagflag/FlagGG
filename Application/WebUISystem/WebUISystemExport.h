#pragma once

#if defined(FlagGG_STATIC_DEFINE) || !defined(WIN32)
#  define WebUISystem_API
#else
#  ifndef WebUISystem_API
#    ifdef WebUISystem_EXPORTS
/* We are building this library */
#      define WebUISystem_API __declspec(dllexport)
#    else
/* We are using this library */
#      define WebUISystem_API __declspec(dllimport)
#    endif
#  endif
#endif
