#include "Core/GenericPlatform.h"
#if PLATFORM_WINDOWS
#include "ImportOpenGL.h"
#include "Core/CryAssert.h"
#include "AsyncFrame/Mutex.h"
#include "Graphics/Window.h"

#include <wgl/wglext.h>

#define GFX_IMPORT(Proto, Func) Proto Func
#include "OpenGLFunc.h"
#undef GFX_IMPORT

typedef PROC(APIENTRYP PFNWGLGETPROCADDRESSPROC) (LPCSTR lpszProc);
typedef BOOL(APIENTRYP PFNWGLMAKECURRENTPROC) (HDC hdc, HGLRC hglrc);
typedef HGLRC(APIENTRYP PFNWGLCREATECONTEXTPROC) (HDC hdc);
typedef BOOL(APIENTRYP PFNWGLDELETECONTEXTPROC) (HGLRC hglrc);

static HMODULE hOpenGLLib = NULL;
static bool glInited = false;
static PFNWGLGETPROCADDRESSPROC _wglGetProcAddress;
static PFNWGLMAKECURRENTPROC _wglMakeCurrent;
static PFNWGLCREATECONTEXTPROC _wglCreateContext;
static PFNWGLDELETECONTEXTPROC _wglDeleteContext;

static PFNWGLCHOOSEPIXELFORMATARBPROC _wglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC _wglCreateContextAttribsARB;
static PFNWGLSWAPINTERVALEXTPROC _wglSwapIntervalEXT;

namespace GL
{

static void InitWGL()
{
	hOpenGLLib = ::LoadLibraryA("opengl32.dll");
	if (hOpenGLLib)
	{
		_wglGetProcAddress = (PFNWGLGETPROCADDRESSPROC)::GetProcAddress(hOpenGLLib, "wglGetProcAddress");
		_wglMakeCurrent = (PFNWGLMAKECURRENTPROC)::GetProcAddress(hOpenGLLib, "wglMakeCurrent");
		_wglCreateContext = (PFNWGLCREATECONTEXTPROC)::GetProcAddress(hOpenGLLib, "wglCreateContext");
		_wglDeleteContext = (PFNWGLDELETECONTEXTPROC)::GetProcAddress(hOpenGLLib, "wglDeleteContext");
	}
}

static void ImportGLInterface()
{
	_wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)_wglGetProcAddress("wglChoosePixelFormatARB");
	_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)_wglGetProcAddress("wglCreateContextAttribsARB");
	_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)_wglGetProcAddress("wglSwapIntervalEXT");

#define GFX_IMPORT(Proto, Func) \
	Func = (Proto)::_wglGetProcAddress(#Func); \
	if (!Func) \
		Func = (Proto)::GetProcAddress(hOpenGLLib, #Func); \
	if (!Func) \
		CRY_ASSERT_MESSAGE(false, #Func);
#include "OpenGLFunc.h"
#undef GFX_IMPORT
	glInited = true;
}

class GLContextWindows;
class GLSwapChainWindows;

static IGLSwapChain* CreateSwapChain(GLContextWindows* glContext, FlagGG::Window* window);

class GLContextWindows : public IGLContext
{
public:
	GLContextWindows()
		: attributes_
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			0, 0,
			0, 0,
		},
		pfd_
		{
			sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
			1,                     // version number  
			PFD_DRAW_TO_WINDOW |   // support window  
			PFD_SUPPORT_OPENGL |   // support OpenGL  
			PFD_DOUBLEBUFFER,      // double buffered  
			PFD_TYPE_RGBA,         // RGBA type  
			24,                    // 24-bit color depth  
			0, 0, 0, 0, 0, 0,      // color bits ignored  
			0,                     // no alpha buffer  
			0,                     // shift bit ignored  
			0,                     // no accumulation buffer  
			0, 0, 0, 0,            // accum bits ignored  
			32,                    // 32-bit z-buffer  
			0,                     // no stencil buffer  
			0,                     // no auxiliary buffer  
			PFD_MAIN_PLANE,        // main layer  
			0,                     // reserved  
			0, 0, 0                // layer masks ignored  
		}
	{
		HWND hWnd = CreateWindowA("STATIC"
			, ""
			, WS_POPUP | WS_DISABLED
			, -32000
			, -32000
			, 0
			, 0
			, NULL
			, NULL
			, GetModuleHandle(NULL)
			, 0
		);

		// https://web.archive.org/web/20190207230357/https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/nf-wingdi-setpixelformat

		HDC hDC = GetDC(hWnd);
		// get the best available match of pixel format for the device context   
		pixelFormat_ = ::ChoosePixelFormat(hDC, &pfd_);

		// make that the pixel format of the device context  
		SetPixelFormat(hDC, pixelFormat_, &pfd_);

		hGlrc_ = _wglCreateContext(hDC);
		_wglMakeCurrent(hDC, hGlrc_);
	}

	~GLContextWindows() override
	{
		_wglDeleteContext(hGlrc_);
	}

	IGLSwapChain* CreateSwapChain(FlagGG::Window* window) override
	{
		return GL::CreateSwapChain(this, window);
	}

	HGLRC hGlrc_;
	Int32 attributes_[9];
	Int32 pixelFormat_;
	PIXELFORMATDESCRIPTOR pfd_;
};

class GLSwapChainWindows : public IGLSwapChain
{
public:
	GLSwapChainWindows(GLContextWindows* glContext, FlagGG::Window* window)
	{
		hWnd_ = (HWND)window->GetHandle();
		hDC_ = ::GetDC(hWnd_);

		::SetPixelFormat(hDC_, glContext->pixelFormat_, &glContext->pfd_);
		hGlrc_ = _wglCreateContextAttribsARB(hDC_, glContext->hGlrc_, glContext->attributes_);
	}

	~GLSwapChainWindows() override
	{
		::ReleaseDC(hWnd_, hDC_);
	}

	void Resize(UInt32 width, UInt32 height) override
	{

	}

	void Swap() override
	{
		::SwapBuffers(hDC_);
	}

	void MakeCurrent() override
	{
		_wglMakeCurrent(hDC_, hGlrc_);
	}

private:
	HWND hWnd_;
	HDC hDC_;
	HGLRC hGlrc_;
};

static IGLSwapChain* CreateSwapChain(GLContextWindows* glContext, FlagGG::Window* window)
{
	IGLSwapChain* glSwapChain = new GLSwapChainWindows(glContext, window);
	return glSwapChain;
}

IGLContext* CreateGLContext()
{
	if (!hOpenGLLib)
		InitWGL();

	IGLContext* glContext = new GLContextWindows();

	if (!glInited)
		ImportGLInterface();

	return glContext;
}

void DestroyGLContext(IGLContext* glContext)
{
	if (!glInited)
		return;
	
	if (glContext)
		delete glContext;
}

}
#endif
