#pragma once

#if defined(URHO3D_STATIC_DEFINE) || !defined(WIN32)
#  define SHADERC_API
#else
#  ifndef SHADERC_API
#    ifdef SHADERC_EXPORTS
/* We are building this library */
#      define SHADERC_API __declspec(dllexport)
#    else
/* We are using this library */
#      define SHADERC_API __declspec(dllimport)
#    endif
#  endif
#endif

namespace bgfx
{
	class SHADERC_API CompileCallback
	{
	public:
		virtual ~CompileCallback() {}

		// 编译shader，错误回调
		virtual void OnError(const char* errMsg) = 0;
	};

	SHADERC_API void SetCompileCallback(CompileCallback* callback);

	SHADERC_API int CommandCompileShader(int _argc, const char* _argv[]);
}
