#include "GfxDeviceOGL.h"
#include "GfxSwapChainOGL.h"
#include "GfxTextureOGL.h"
#include "GfxBufferOGL.h"
#include "GfxShaderOGL.h"
#include "GfxProgramOGL.h"

namespace FlagGG
{

GfxDeviceOpenGL::GfxDeviceOpenGL()
	: GfxDevice()
{

}

GfxDeviceOpenGL::~GfxDeviceOpenGL()
{

}

void GfxDeviceOpenGL::SetFrameBuffer()
{
	GLFrameBufferKey key;
	for (UInt32 i = 0u; i < MAX_RENDERTARGET_COUNT; ++i)
	{
		SharedPtr<GfxRenderSurfaceOpenGL> renderTargetGL = DynamicCast<GfxRenderSurfaceOpenGL>(renderTargets_[i]);
		key[i] = renderTargetGL ? renderTargetGL->GetOGLRenderBuffer() : 0u;
	}
	SharedPtr<GfxRenderSurfaceOpenGL> depthStencilGL = DynamicCast<GfxRenderSurfaceOpenGL>(depthStencil_);
	key[MAX_RENDERTARGET_COUNT] = depthStencilGL ? depthStencilGL->GetOGLRenderBuffer() : 0u;
	key.CalculateHash();
	
	auto it = frameBufferMap_.Find(key);
	if (it != frameBufferMap_.End())
	{
		GL::BindFramebuffer(GL_FRAMEBUFFER, it->second_);
		return;
	}

	GLuint oglFrameBuffer = 0u;
	GL::GenFrameBuffers(1, &oglFrameBuffer);
	GL::BindFramebuffer(GL_FRAMEBUFFER, oglFrameBuffer);

	frameBufferMap_.Insert(MakePair(key, oglFrameBuffer));
}

void GfxDeviceOpenGL::Clear(ClearTargetFlags flags, const Color& color/* = Color::TRANSPARENT_BLACK*/, float depth/* = 1.0f*/, unsigned stencil/* = 0*/)
{
	if (renderTargetDirty_)
	{
		SetFrameBuffer();
	}
}

void GfxDeviceOpenGL::PrepareDraw()
{

}

void GfxDeviceOpenGL::Draw(UInt32 vertexStart, UInt32 vertexCount)
{
	PrepareDraw();


}

void GfxDeviceOpenGL::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	UInt32 indexStartByte = indexStart * indexBuffer_->GetDesc().stride_;
	GL::DrawElements(0, indexCount, 0, (void*)(uintptr_t)indexStartByte);
}

void GfxDeviceOpenGL::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount)
{
	UInt32 indexStartByte = indexStart * indexBuffer_->GetDesc().stride_;
	GL::DrawElementsInstanced(0, indexCount, 0, (void*)(uintptr_t)indexStartByte, instanceCount);
}

void GfxDeviceOpenGL::Flush()
{

}

GfxSwapChain* GfxDeviceOpenGL::CreateSwapChain(Window* window)
{
	return new GfxSwapChainOpenGL(window);
}

GfxTexture* GfxDeviceOpenGL::CreateTexture()
{
	return new GfxTextureOpenGL();
}

GfxBuffer* GfxDeviceOpenGL::CreateBuffer()
{
	return new GfxBufferOpenGL();
}

GfxShader* GfxDeviceOpenGL::CreateShader()
{
	return new GfxShaderOpenGL();
}

GfxProgram* GfxDeviceOpenGL::CreateProgram()
{
	return new GfxProgramOpenGL();
}

}
