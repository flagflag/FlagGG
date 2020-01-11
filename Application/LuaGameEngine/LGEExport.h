#pragma once

#if defined(LuaGameEngine_STATIC_DEFINE) || !defined(WIN32)
#  define LuaGameEngine_API
#else
#  ifndef LuaGameEngine_API
#    ifdef LuaGameEngine_EXPORTS
/* We are building this library */
#      define LuaGameEngine_API __declspec(dllexport)
#    else
/* We are using this library */
#      define LuaGameEngine_API __declspec(dllimport)
#    endif
#  endif
#endif
