#include "GfxDeviceMetal.h"
#include "GfxSwapChainMetal.h"
#include "GfxTextureMetal.h"
#include "GfxBufferMetal.h"
#include "GfxShaderMetal.h"
#include "GfxProgramMetal.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

static const mtlpp::CullMode metalCullMode[] =
{
	mtlpp::CullMode::None,
	mtlpp::CullMode::Front,
	mtlpp::CullMode::Back,
};

static const mtlpp::TriangleFillMode metalFillMode[] =
{
	mtlpp::TriangleFillMode::Lines,
	mtlpp::TriangleFillMode::Fill,
};

static const mtlpp::BlendFactor metalSourceRgbBlendFactor[] =
{
	mtlpp::BlendFactor::One,
};

static const mtlpp::BlendFactor metalDestRgbBlendFactor[] =
{
	mtlpp::BlendFactor::One,
};

static const mtlpp::BlendOperation metalRgbBlendOpt[] =
{
	mtlpp::BlendOperation::Add,
};

static const mtlpp::BlendFactor metalSourceAlphaBlendFactor[] =
{
	mtlpp::BlendFactor::One,
};

static const mtlpp::BlendFactor metalDestAlphaBlendFactor[] =
{
	mtlpp::BlendFactor::One,
};

static const mtlpp::BlendOperation metalAlphaBlendOpt[] =
{
	mtlpp::BlendOperation::Add,
};

static const mtlpp::CompareFunction metalCompareFunction[] =
{
	mtlpp::CompareFunction::Never,
	mtlpp::CompareFunction::Less,
	mtlpp::CompareFunction::Equal,
	mtlpp::CompareFunction::LessEqual,
	mtlpp::CompareFunction::Greater,
	mtlpp::CompareFunction::NotEqual,
	mtlpp::CompareFunction::GreaterEqual,
	mtlpp::CompareFunction::Always,
};

static const mtlpp::StencilOperation metalStencilOperation[] =
{
	mtlpp::StencilOperation::Keep,
	mtlpp::StencilOperation::Zero,
	mtlpp::StencilOperation::Replace,
	mtlpp::StencilOperation::IncrementClamp,
	mtlpp::StencilOperation::DecrementClamp,
	mtlpp::StencilOperation::Invert,
	mtlpp::StencilOperation::IncrementWrap,
	mtlpp::StencilOperation::DecrementWrap,
};

static const mtlpp::PrimitiveType metalPrimitiveType[] =
{
	mtlpp::PrimitiveType::Triangle,
	mtlpp::PrimitiveType::Line,
};

GfxDeviceMetal::GfxDeviceMetal()
	: mtlDevice_(mtlpp::Device::CreateSystemDefaultDevice())
{

}

GfxDeviceMetal::~GfxDeviceMetal()
{

}

void GfxDeviceMetal::Clear(ClearTargetFlags flags, const Color& color, float depth, unsigned stencil)
{

}

void GfxDeviceMetal::Draw(UInt32 vertexStart, UInt32 vertexCount)
{
	PrepareDraw();

	mtlRenderCommandEncoder_.Draw(metalPrimitiveType[primitiveType_], vertexStart, vertexCount);
}

void GfxDeviceMetal::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	PrepareDraw();

	mtlRenderCommandEncoder_.DrawIndexed(metalPrimitiveType[primitiveType_], indexCount, mtlpp::IndexType::UInt16, indexBuffer_->Cast<GfxBufferMetal>()->GetMetalBuffer(), indexStart);
}

void GfxDeviceMetal::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount)
{
	PrepareDraw();

	mtlRenderCommandEncoder_.DrawIndexed(metalPrimitiveType[primitiveType_], indexCount, mtlpp::IndexType::UInt16, indexBuffer_->Cast<GfxBufferMetal>()->GetMetalBuffer(), indexStart, instanceCount);
}

void GfxDeviceMetal::Flush()
{
	mtlRenderCommandEncoder_.EndEncoding();
}

GfxSwapChain* GfxDeviceMetal::CreateSwapChain(Window* window)
{
	return new GfxSwapChainMetal(window);
}

GfxTexture* GfxDeviceMetal::CreateTexture()
{
	return new GfxTextureMetal();
}

GfxBuffer* GfxDeviceMetal::CreateBuffer()
{
	return new GfxBufferMetal();
}

GfxShader* GfxDeviceMetal::CreateShader()
{
	return new GfxShaderMetal();
}

GfxProgram* GfxDeviceMetal::CreateProgram()
{
	return new GfxProgramMetal();
}

void GfxDeviceMetal::PrepareDraw()
{
	PrepareRenderPassAttachments();

	PrepareRenderPiplineState();

	PrepareDepthStencilState();

	CopyShaderParameterToBuffer(currentProgram_->GetContantBufferVariableDesc(VS), vsConstantBuffer_);
	CopyShaderParameterToBuffer(currentProgram_->GetContantBufferVariableDesc(PS), psConstantBuffer_);

	UInt32 maxUniformRegisterIdx = 0;

	for (auto& it : currentProgram_->GetContantBufferVariableDesc(VS))
	{
		mtlRenderCommandEncoder_.SetVertexBuffer(vsConstantBuffer_[it.first_].GetMetalBuffer(), 0, it.first_);
		maxUniformRegisterIdx = Max(maxUniformRegisterIdx, it.first_);
	}

	for (auto& it : currentProgram_->GetContantBufferVariableDesc(PS))
	{
		mtlRenderCommandEncoder_.SetFragmentBuffer(psConstantBuffer_[it.first_].GetMetalBuffer(), 0, it.first_);
	}

	for (UInt32 idx = 0; idx < vertexBuffers_.Size(); ++idx)
	{
		mtlRenderCommandEncoder_.SetVertexBuffer(vertexBuffers_[idx]->Cast<GfxBufferMetal>()->GetMetalBuffer(), 0, maxUniformRegisterIdx + idx);
	}

	if (texturesDirty_)
	{
		for (auto& it : currentProgram_->GetTextureDesc(VS))
		{
			auto* textureMetal = RTTICast<GfxTextureMetal>(textures_[it.first_]);
			if (textureMetal)
			{
				mtlRenderCommandEncoder_.SetVertexTexture(textureMetal->GetMetalTexture(), it.first_);
			}
		}

		for (auto& it : currentProgram_->GetTextureDesc(PS))
		{
			auto* textureMetal = RTTICast<GfxTextureMetal>(textures_[it.first_]);
			if (textureMetal)
			{
				mtlRenderCommandEncoder_.SetFragmentTexture(textureMetal->GetMetalTexture(), it.first_);
			}
		}

		texturesDirty_ = false;
	}
}

void GfxDeviceMetal::PrepareRenderPassAttachments()
{
	if (renderTargetDirty_ || depthStencilDirty_)
	{
		mtlpp::RenderPassDescriptor mtlRenderPassDesc;
		Int32 rtCount = MAX_RENDERTARGET_COUNT;
		for (; rtCount; --rtCount)
		{
			if (renderTargets_[rtCount - 1])
				break;
		}
		mtlRenderPassDesc.SetRenderTargetArrayLength(rtCount);
		ns::Array<mtlpp::RenderPassColorAttachmentDescriptor> colorAttachments = mtlRenderPassDesc.GetColorAttachments();
		for (UInt32 i = 0; i < rtCount; ++i)
		{
			mtlpp::RenderPassColorAttachmentDescriptor& colorAttachment = colorAttachments[i];
			colorAttachment.SetTexture(renderTargets_[i]->GetOwnerTexture()->Cast<GfxTextureMetal>()->GetMetalTexture());
			colorAttachment.SetLoadAction(mtlpp::LoadAction::DontCare);
			colorAttachment.SetStoreAction(mtlpp::StoreAction::DontCare);
		}

		mtlpp::RenderPassDepthAttachmentDescriptor depthAttachment;
		depthAttachment.SetTexture(depthStencil_->GetOwnerTexture()->Cast<GfxTextureMetal>()->GetMetalTexture());
		depthAttachment.SetLoadAction(mtlpp::LoadAction::DontCare);
		depthAttachment.SetStoreAction(mtlpp::StoreAction::DontCare);
		mtlRenderPassDesc.SetDepthAttachment(depthAttachment);

		mtlpp::RenderPassStencilAttachmentDescriptor stencilAttachment;
		stencilAttachment.SetLoadAction(mtlpp::LoadAction::DontCare);
		stencilAttachment.SetStoreAction(mtlpp::StoreAction::DontCare);
		mtlRenderPassDesc.SetStencilAttachment(stencilAttachment);

		mtlRenderCommandEncoder_ = mtlCommandBuffer_.RenderCommandEncoder(mtlRenderPassDesc);

		renderTargetDirty_ = false;
		depthStencilDirty_ = false;
	}
}

void GfxDeviceMetal::PrepareRenderPiplineState()
{
	if (rasterizerStateDirty_)
	{
		mtlRenderCommandEncoder_.SetCullMode(metalCullMode[rasterizerState_.cullMode_]);
		mtlRenderCommandEncoder_.SetTriangleFillMode(metalFillMode[rasterizerState_.fillMode_]);
		mtlRenderCommandEncoder_.SetDepthBias(rasterizerState_.depthBias_, rasterizerState_.slopeScaledDepthBias_, 0.0f);
	}

	if (rasterizerStateDirty_ || shaderDirty_)
	{
		UInt32 stateHash = rasterizerState_.GetHash();
		UInt64 pointValue = (UInt64)vertexShader_.Get();
		for (UInt32 i = 0; i < 8; ++i)
		{
			stateHash = SDBM_Hash(stateHash, pointValue & 0xFF);
			pointValue <<= 8;
		}
		pointValue = (UInt64)pixelShader_.Get();
		for (UInt32 i = 0; i < 8; ++i)
		{
			stateHash = SDBM_Hash(stateHash, pointValue & 0xFF);
			pointValue <<= 8;
		}

		auto it = renderPiplineStateMap_.Find(stateHash);
		if (it == renderPiplineStateMap_.End())
		{
			mtlpp::RenderPipelineDescriptor mtlRenderPiplineDesc;
			mtlRenderPiplineDesc.SetVertexFunction(vertexShader_->Cast<GfxShaderMetal>()->GetMetalFunction());
			mtlRenderPiplineDesc.SetFragmentFunction(pixelShader_->Cast<GfxShaderMetal>()->GetMetalFunction());
			ns::Array<mtlpp::RenderPipelineColorAttachmentDescriptor> colorAttachments = mtlRenderPiplineDesc.GetColorAttachments();
			for (UInt32 i = 0; i < colorAttachments.GetSize(); ++i)
			{
				auto& colorAttachment = colorAttachments[i];
				colorAttachment.SetPixelFormat(GfxTextureMetal::ToMetalPixelFormat(renderTargets_[i]->GetOwnerTexture()->Cast<GfxTexture>()->GetDesc().format_));

				if (rasterizerState_.blendMode_ != BLEND_REPLACE)
				{
					colorAttachment.SetBlendingEnabled(true);
					colorAttachment.SetSourceRgbBlendFactor(metalSourceRgbBlendFactor[rasterizerState_.blendMode_]);
					colorAttachment.SetDestinationRgbBlendFactor(metalDestRgbBlendFactor[rasterizerState_.blendMode_]);
					colorAttachment.SetRgbBlendOperation(metalRgbBlendOpt[rasterizerState_.blendMode_]);
					colorAttachment.SetSourceAlphaBlendFactor(metalSourceAlphaBlendFactor[rasterizerState_.blendMode_]);
					colorAttachment.SetDestinationAlphaBlendFactor(metalDestAlphaBlendFactor[rasterizerState_.blendMode_]);
					colorAttachment.SetAlphaBlendOperation(metalAlphaBlendOpt[rasterizerState_.blendMode_]);
				}
				else
				{
					colorAttachment.SetBlendingEnabled(false);
				}

				if (rasterizerState_.colorWrite_)
				{
					UInt32 colorWriteMask = (UInt32)mtlpp::ColorWriteMask::Red | (UInt32)mtlpp::ColorWriteMask::Green | (UInt32)mtlpp::ColorWriteMask::Blue;
					if (rasterizerState_.blendMode_ != BLEND_REPLACE)
						colorWriteMask |= (UInt32)mtlpp::ColorWriteMask::Alpha;
					colorAttachment.SetWriteMask((mtlpp::ColorWriteMask)colorWriteMask);
				}
				else
				{
					colorAttachment.SetWriteMask(mtlpp::ColorWriteMask::None);
				}
			}
			mtlRenderPiplineDesc.SetDepthAttachmentPixelFormat(GfxTextureMetal::ToMetalPixelFormat(depthStencil_->GetOwnerTexture()->GetDesc().format_));
			mtlRenderPiplineDesc.SetStencilAttachmentPixelFormat(GfxTextureMetal::ToMetalPixelFormat(depthStencil_->GetOwnerTexture()->GetDesc().format_));

			UInt32 piplineOptions = (UInt32)mtlpp::PipelineOption::ArgumentInfo | (UInt32)mtlpp::PipelineOption::BufferTypeInfo;	
			mtlpp::RenderPipelineReflection reflection;
			ns::Error error;
			
			mtlpp::RenderPipelineState mtlRenderPiplineState = mtlDevice_.NewRenderPipelineState(mtlRenderPiplineDesc, (mtlpp::PipelineOption)piplineOptions, &reflection, &error);
			if (error.GetCode() == 0)
				throw "Failed to Create RenderPipelineState.";


			SharedPtr<GfxProgramMetal> program(new GfxProgramMetal());
			program->Link(vertexShader_, pixelShader_);
			program->ProcessReflectIfNeed(&reflection);

			it = renderPiplineStateMap_.Insert(MakePair(stateHash, RenderPiplineStateAndShaders{ mtlRenderPiplineState, program }));
		}

		mtlRenderCommandEncoder_.SetRenderPipelineState(it->second_.mtlRenderPiplineState_);
		currentProgram_ = it->second_.program_;

		rasterizerStateDirty_ = false;
		shaderDirty_ = false;
	}
}

void GfxDeviceMetal::PrepareDepthStencilState()
{
	if (depthStencilStateDirty_)
	{
		UInt32 stateHash = depthStencilState_.GetHash();

		auto it = depthStencilStateMap_.Find(stateHash);
		if (it == depthStencilStateMap_.End())
		{
			mtlpp::DepthStencilDescriptor mtlDepthStencilDesc;
			mtlDepthStencilDesc.SetDepthCompareFunction(metalCompareFunction[depthStencilState_.depthTestMode_]);
			mtlDepthStencilDesc.SetDepthWriteEnabled(depthStencilState_.depthWrite_);

			mtlpp::StencilDescriptor mtlStencilDesc;
			mtlStencilDesc.SetStencilCompareFunction(metalCompareFunction[depthStencilState_.stencilTest_]);
			mtlStencilDesc.SetStencilFailureOperation(metalStencilOperation[depthStencilState_.stencilFailOp_]);
			mtlStencilDesc.SetDepthFailureOperation(metalStencilOperation[depthStencilState_.depthFailOp_]);
			mtlStencilDesc.SetDepthStencilPassOperation(metalStencilOperation[depthStencilState_.depthStencilPassOp_]);
			mtlStencilDesc.SetReadMask(depthStencilState_.stencilReadMask_);
			mtlStencilDesc.SetWriteMask(depthStencilState_.stencilWriteMask_);
			mtlDepthStencilDesc.SetFrontFaceStencil(mtlStencilDesc);
			mtlDepthStencilDesc.SetBackFaceStencil(mtlStencilDesc);

			mtlpp::DepthStencilState mtlDepthStencilState = mtlDevice_.NewDepthStencilState(mtlDepthStencilDesc);
			it = depthStencilStateMap_.Insert(MakePair(stateHash, mtlDepthStencilState));
		}

		mtlRenderCommandEncoder_.SetDepthStencilState(it->second_);

		depthStencilStateDirty_ = false;
	}
}

void GfxDeviceMetal::CopyShaderParameterToBuffer(const HashMap<UInt32, MetalConstanceBufferDesc>& bufferDesc, GfxBuffer* bufferArray)
{

}

}
