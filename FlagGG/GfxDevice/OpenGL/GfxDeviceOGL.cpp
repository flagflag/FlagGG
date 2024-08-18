#include "GfxDeviceOGL.h"
#include "GfxSwapChainOGL.h"
#include "GfxTextureOGL.h"
#include "GfxBufferOGL.h"
#include "GfxShaderOGL.h"
#include "GfxProgramOGL.h"
#include "GfxDevice/VertexDescFactory.h"
#include "Graphics/ShaderParameter.h"

namespace FlagGG
{

struct OglVertexElements
{
	GLint components_;
	GLenum type_;
};
static const OglVertexElements oglVertexElements[] =
{
	{ 1, GL_INT   },          // VE_INT
	{ 1, GL_FLOAT },          // VE_FLOAT
	{ 2, GL_FLOAT  },         // VE_VECTOR2
	{ 3, GL_FLOAT },          // VE_VECTOR3
	{ 4, GL_FLOAT },          // VE_VECTOR4
	{ 4, GL_UNSIGNED_BYTE },  // VE_UBYTE4
	{ 4, GL_UNSIGNED_BYTE },  // VE_UBYTE4_UNORM
};

GfxDeviceOpenGL::GfxDeviceOpenGL()
	: GfxDevice()
{
	glContext_ = GL::CreateGLContext();

#if _DEBUG
	GL::Enable(GL_DEBUG_OUTPUT);
	GL::DebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (type == GL_DEBUG_TYPE_ERROR)
			FLAGGG_LOG_STD_ERROR("GL ERROR: type = %d, severity = %d, message = %s", type, severity, message);
		else
			FLAGGG_LOG_STD_DEBUG("GL DEBUG: type = %dx, severity = %d, message = %s", type, severity, message);
	}, glContext_);
#endif
}

GfxDeviceOpenGL::~GfxDeviceOpenGL()
{
#if _DEBUG
	GL::DebugMessageCallback(nullptr, nullptr);
#endif
}

void GfxDeviceOpenGL::SetFrameBuffer()
{
	GLFrameBufferKey key;

	{
		for (UInt32 i = 0u; i < MAX_RENDERTARGET_COUNT; ++i)
		{
			SharedPtr<GfxRenderSurfaceOpenGL> renderTargetGL(RTTICast<GfxRenderSurfaceOpenGL>(renderTargets_[i]));
			key[i] = renderTargetGL ? renderTargetGL->GetOGLRenderBuffer() : 0u;
		}
		SharedPtr<GfxRenderSurfaceOpenGL> depthStencilGL(RTTICast<GfxRenderSurfaceOpenGL>(depthStencil_));
		key[MAX_RENDERTARGET_COUNT] = depthStencilGL ? depthStencilGL->GetOGLRenderBuffer() : 0u;
		key.CalculateHash();
	}
	
	auto it = frameBufferMap_.Find(key);
	if (it != frameBufferMap_.End())
	{
		GL::BindFramebuffer(GL_FRAMEBUFFER, it->second_);
		return;
	}

	GLuint oglFrameBuffer = 0u;
	GL::GenFramebuffers(1, &oglFrameBuffer);
	GL::BindFramebuffer(GL_FRAMEBUFFER, oglFrameBuffer);

	{
		for (UInt32 i = 0u; i < MAX_RENDERTARGET_COUNT; ++i)
		{
			SharedPtr<GfxRenderSurfaceOpenGL> renderTargetGL(RTTICast<GfxRenderSurfaceOpenGL>(renderTargets_[i]));
			if (renderTargetGL && renderTargetGL->GetOGLRenderBuffer())
			{
				GL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, renderTargetGL->GetOGLRenderBuffer());
			}
		}
		SharedPtr<GfxRenderSurfaceOpenGL> depthStencilGL(RTTICast<GfxRenderSurfaceOpenGL>(depthStencil_));
		if (depthStencilGL && depthStencilGL->GetOGLRenderBuffer())
		{
			GL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilGL->GetOGLRenderBuffer());
		}
	}

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
		if (vertexBuffers_.Size())
		{
			auto vertexBufferOGL = RTTICast<GfxBufferOpenGL>(vertexBuffers_[0]);
			if (vertexBufferOGL)
				GL::BindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->GetOGLBuffer());
		}

		vertexBufferDirty_ = false;
	}

	if (indexBufferDirty_)
	{
		auto indexBufferOGL = RTTICast<GfxBufferOpenGL>(indexBuffer_);
		if (indexBufferOGL)
			GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->GetOGLBuffer());

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

		currentProgram_ = currentProgram;

		shaderDirty_ = false;
	}

	if (vertexDescDirty_)
	{
		BindOGLInputLayout(vertexDesc_);

		vertexDescDirty_ = false;
	}

	SetShaderParameters(currentProgram_->GetUniformVariableDescs());

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
			auto textureOGL = RTTICast<GfxTextureOpenGL>(textures_[i]);
			if (textureOGL)
			{
				GL::ActiveTexture(GL_TEXTURE0 + i);
				GL::BindTexture(textureOGL->GetOGLTarget(), textureOGL->GetOGLTexture());

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

void GfxDeviceOpenGL::SetShaderParameters(const Vector<OGLShaderUniformVariableDesc>& uniformVariableDesc)
{
	typedef void (*UniformivFunc)(GLint location, GLsizei count, const GLint* value);
	typedef void (*UniformfvFunc)(GLint location, GLsizei count, const GLfloat* value);

	// 懒得写了，暂时只支持int数组，int2 int3 int4这种让它崩溃
	static UniformivFunc UNIFORMIV_FUNCS[] =
	{
		&GL::Uniform1iv,
		nullptr,
		nullptr,
		nullptr,
	};

	static UniformfvFunc UNIFORMFV_FUNCS[] =
	{
		&GL::Uniform1fv,
		&GL::Uniform2fv,
		&GL::Uniform3fv,
		&GL::Uniform4fv,
	};

	auto SetParam = [&](ShaderParameters& shaderParam, const OGLShaderUniformVariableDesc& desc)
	{
		if (desc.vectorSize_ == 0 || desc.vectorSize_ > 4)
			return;

		UInt32 dataSize = 4u * desc.vectorSize_;
		if (dataSize < tempBuffer_.Size())
			tempBuffer_.Resize(dataSize);

		if (shaderParam.ReadParameter(desc.name_, &tempBuffer_[0], dataSize))
		{
			switch (desc.type_)
			{
			case GL_INT:
			{
				UNIFORMIV_FUNCS[desc.vectorSize_](desc.location_, 1, (const GLint*)tempBuffer_.Buffer());
			}
			break;

			case GL_FLOAT:
			{
				UNIFORMFV_FUNCS[desc.vectorSize_ - 1](desc.location_, desc.vectorSize_, (const GLfloat*)tempBuffer_.Buffer());
			}
			break;
			}
		}
	};

	for (auto& desc : uniformVariableDesc)
	{
		if (engineShaderParameters_)
			SetParam(*engineShaderParameters_, desc);

		if (materialShaderParameters_)
			SetParam(*materialShaderParameters_, desc);
	}
}

void GfxDeviceOpenGL::BindOGLInputLayout(VertexDescription* vertxDesc)
{
	if (currentProgram_)
	{
		const auto& vertexInputDescs = currentProgram_->GetVertexInputDescs();
		const PODVector<VertexElement>& elements = vertxDesc->GetElements();

		for (const auto& element : elements)
		{
			const char* semName = VERTEX_ELEMENT_SEM_NAME[element.vertexElementSemantic_];

			auto it = vertexInputDescs.Find(semName);
			if (it != vertexInputDescs.End())
			{
				auto& desc = it->second_;

				GL::EnableVertexAttribArray(desc.location_);
				GL::VertexAttribDivisor(desc.location_, 0);
				GL::VertexAttribPointer(desc.location_,
					oglVertexElements[element.vertexElementType_].components_,
					oglVertexElements[element.vertexElementType_].type_,
					false,
					VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_],
					nullptr);
			}
		}
	}
}

void GfxDeviceOpenGL::UnbindOGLInputLayout(VertexDescription* vertxDesc)
{

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
	GLenum indexFormat = indexBuffer_->GetDesc().stride_ == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	GL::DrawElements(GetOGLPrimitive(primitiveType_), indexCount, indexFormat, (void*)(uintptr_t)indexStartByte);
}

void GfxDeviceOpenGL::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount)
{
	PrepareDraw();

	UInt32 indexStartByte = indexStart * indexBuffer_->GetDesc().stride_;
	GLenum indexFormat = indexBuffer_->GetDesc().stride_ == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	GL::DrawElementsInstanced(GetOGLPrimitive(primitiveType_), indexCount, indexFormat, (void*)(uintptr_t)indexStartByte, instanceCount);
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
