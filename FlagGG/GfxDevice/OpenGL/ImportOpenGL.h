#pragma once

#include <gl/glcorearb.h>
#include <gl/glext.h>
#include "Container/RefCounted.h"

#define GFX_IMPORT(Proto, Func) extern Proto Func
#include "OpenGLFunc.h"
#undef GFX_IMPORT

namespace FlagGG
{

class Window;

}

namespace GL
{

class IGLSwapChain : public FlagGG::RefCounted
{
public:
	virtual void Resize(UInt32 width, UInt32 height) = 0;

	virtual void Swap() = 0;

	virtual void MakeCurrent() = 0;
};

class IGLContext : public FlagGG::RefCounted
{
public:
	virtual IGLSwapChain* CreateSwapChain(FlagGG::Window* window) = 0;
};

extern IGLContext* CreateGLContext();

extern void DestroyGLContext(IGLContext* glContext);

}
