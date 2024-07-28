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
	glContext_ = GL::CreateGLContext();
}

GfxDeviceOpenGL::~GfxDeviceOpenGL()
{

}

void GfxDeviceOpenGL::SetFrameBuffer()
{
	GLFrameBufferKey key;
	for (UInt32 i = 0u; i < MAX_RENDERTARGET_COUNT; ++i)
	{
		SharedPtr<GfxRenderSurfaceOpenGL> renderTargetGL(RTTICast<GfxRenderSurfaceOpenGL>(renderTargets_[i]));
		key[i] = renderTargetGL ? renderTargetGL->GetOGLRenderBuffer() : 0u;
	}
	SharedPtr<GfxRenderSurfaceOpenGL> depthStencilGL(RTTICast<GfxRenderSurfaceOpenGL>(depthStencil_));
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

		renderTargetDirty_ = false;
	}
}

void GfxDeviceOpenGL::PrepareRasterizerState()
{

}

void GfxDeviceOpenGL::PrepareDepthStencilState()
{

}

void GfxDeviceOpenGL::PrepareDraw()
{
	PrepareRasterizerState();

	PrepareDepthStencilState();

	if (vertexBufferDirty_)
	{


		vertexBufferDirty_ = false;
	}

	if (indexBufferDirty_)
	{

		indexBufferDirty_ = false;
	}

	if (shaderDirty_)
	{
		auto& currentProgram = programMap_[MakePair(WeakPtr<GfxShader>(vertexShader_), WeakPtr<GfxShader>(pixelShader_))];
		if (!currentProgram)
		{
			currentProgram = MakeShared<GfxProgramOpenGL>();
			currentProgram->Link(vertexShader_, pixelShader_);
		}

		GL::UseProgram(currentProgram->GetOGLProgram());

		shaderDirty_ = false;
	}

	if (vertexDescDirty_)
	{


		vertexDescDirty_ = false;
	}

	if (renderTargetDirty_ || depthStencilDirty_)
	{
		SetFrameBuffer();

		renderTargetDirty_ = false;
		depthStencilDirty_ = false;
	}

	if (texturesDirty_ || samplerDirty_)
	{
		for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
		{
			auto currentTexture = RTTICast<GfxTextureOpenGL>(textures_[i]);
			if (currentTexture)
			{
				GL::ActiveTexture(GL_TEXTURE0 + i);
				GL::BindTexture(currentTexture->GetOGLTarget(), currentTexture->GetOGLTexture());

				// TODO: Set sample state
			}
		}

		texturesDirty_ = false;
		samplerDirty_ = false;
	}

	if (viewportDirty_)
	{
		GL::Viewport(viewport_.Left(), viewport_.Top(), viewport_.Width(), viewport_.Height());

		viewportDirty_ = false;
	}
}

static GLenum GetOGLPrimitive(PrimitiveType primitiveType)
{
	switch (primitiveType)
	{
	case PRIMITIVE_LINE:
		return GL_LINES;
		break;

	case PRIMITIVE_TRIANGLE:
		return GL_TRIANGLES;
		break;
	}

	return GL_ZERO;
}

void GfxDeviceOpenGL::Draw(UInt32 vertexStart, UInt32 vertexCount)
{
	PrepareDraw();

	GL::DrawArrays(GetOGLPrimitive(primitiveType_), vertexStart, vertexCount);
}

void GfxDeviceOpenGL::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	PrepareDraw();

	UInt32 indexStartByte = indexStart * indexBuffer_->GetDesc().stride_;
	GL::DrawElements(GetOGLPrimitive(primitiveType_), indexCount, 0, (void*)(uintptr_t)indexStartByte);
}

void GfxDeviceOpenGL::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount)
{
	PrepareDraw();

	UInt32 indexStartByte = indexStart * indexBuffer_->GetDesc().stride_;
	GL::DrawElementsInstanced(GetOGLPrimitive(primitiveType_), indexCount, 0, (void*)(uintptr_t)indexStartByte, instanceCount);
}

void GfxDeviceOpenGL::Flush()
{
	GL::Flush();
}

GfxSwapChain* GfxDeviceOpenGL::CreateSwapChain(Window* window)
{
	return new GfxSwapChainOpenGL(glContext_, window);
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
