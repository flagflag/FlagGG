#include "UISystem.h"
#include "Core/EventManager.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "GfxDevice/GfxDevice.h"
#include "Resource/ResourceCache.h"
#include "UI/UIEvents.h"
#include "Memory/Memory.h"

namespace FlagGG
{

struct UIBatchRenderData
{
	// 顶点buffer
	VertexVector vertexVector_;
	// 渲染批次
	Vector<SharedPtr<Batch>> batches_;

	// 渲染surface
	SharedPtr<GfxRenderSurface> renderSurface_;
	// 渲染区域
	Rect viewport_;
	// 主动调用渲染
	bool manualRender_{};

	// 顶点数据
	SharedPtr<VertexBuffer> vertexBuffer_;
	// ui贴图
	Vector<SharedPtr<Texture>> textures_;
	// vertex shader
	SharedPtr<Shader> vertexShader_;
	// pixel shader
	SharedPtr<Shader> pixelShader_;

	// 渲染webkit-ui
	bool webKitRendering_{};
	// 背景透明度（只有webkit有效）
	Real backgroundTransparency_{ 1.0f };

	void BuildVertexBuffer();

	void Render(Texture2D* defaultTexture, ShaderParameters* materialParameters, Real backgroundTransparency);
};

UISystem::UISystem()
{
	defaultTexture_ = new Texture2D();
	defaultTexture_->SetNumLevels(1u);
	defaultTexture_->SetSize(1, 1, TEXTURE_FORMAT_RGBA8);
	UInt32 colorValue = Color::WHITE.ToUInt();
	defaultTexture_->SetData(0, 0, 0, 1, 1, &colorValue);

	materialParameters_ = new ShaderParameters();
	materialParameters_->AddParametersDefine<Real>("transparency");

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::PRERENDER_UPDATE, UISystem::HandleRenderUpdate, this));
}

UISystem::~UISystem()
{

}

void UISystem::Initialize()
{

}

void UISystem::HandleRenderUpdate(Real timeStep)
{
	renderUITreesCache_.Clear();

	GetSubsystem<EventManager>()->SendEvent<UIEvent::GATHER_RENDER_UI_TREE_HANDLER>(renderUITreesCache_);

	for (auto& it : renderUITreesCache_)
	{
		it.uiRoot_->CalculateLayout(it.viewport_.Width(), it.viewport_.Height());

		auto& batchRenderData = batchRenderDataMap_[it.uiRoot_];
		batchRenderData.vertexVector_.Clear();
		batchRenderData.batches_.Clear();
		batchRenderData.renderSurface_ = it.renderSurface_;
		batchRenderData.viewport_ = it.viewport_;
		batchRenderData.manualRender_ = it.manualRender_;
		batchRenderData.webKitRendering_ = it.webKitRendering_;
		batchRenderData.backgroundTransparency_ = it.backgroundTransparency_;

		UpdateBatches(it.uiRoot_, &batchRenderData.vertexVector_, batchRenderData.batches_);

		batchRenderData.BuildVertexBuffer();
	}

	renderUITreesCache_.Clear();
}

void UISystem::UpdateBatches(UIElement* element, VertexVector* vertexVector, Vector<SharedPtr<Batch>>& batches)
{
	element->UpdateBatch(vertexVector, batches);

	for (auto& it : element->GetChildren())
	{
		UpdateBatches(it, vertexVector, batches);
	}
}

void UISystem::Render(UIElement* uiElement)
{
	auto* batchRenderData = batchRenderDataMap_.TryGetValue(SharedPtr<UIElement>(uiElement));
	if (!batchRenderData)
		return;
;
	batchRenderData->Render(defaultTexture_, materialParameters_, batchRenderData->backgroundTransparency_);
}

void UISystem::RenderWebKit(GfxRenderSurface* renderSurface, const Rect& viewport, const Vector<SharedPtr<Batch>>& uiBatches)
{
	static UIBatchRenderData batchRenderData;
	batchRenderData.renderSurface_ = renderSurface;
	batchRenderData.viewport_ = viewport;
	batchRenderData.batches_ = uiBatches;
	batchRenderData.webKitRendering_ = true;
	batchRenderData.BuildVertexBuffer();
	batchRenderData.Render(defaultTexture_, materialParameters_, 1.0f);
}

void UIBatchRenderData::BuildVertexBuffer()
{
	if (!vertexBuffer_)
	{
		vertexBuffer_ = new VertexBuffer();
	}

	UInt32 vertexCount = 0u;

	for (const auto& batch : batches_)
	{
		vertexCount += batch->GetVertexCount();
	}

	if (vertexCount == 0)
		return;

	vertexBuffer_->SetSize(vertexCount, webKitRendering_ ? BatchWebKit::GetVertexElements() : Batch2D::GetVertexElements());
	char* data = (char*)vertexBuffer_->Lock(0, vertexBuffer_->GetVertexCount());

	for (const auto& batch : batches_)
	{
		Memory::Memcpy(data, batch->GetVertexs(), batch->GetVertexCount() * batch->GetVertexSize());
		data += (batch->GetVertexCount() * vertexBuffer_->GetVertexSize());
	}

	vertexBuffer_->Unlock();
}

void UIBatchRenderData::Render(Texture2D* defaultTexture, ShaderParameters* materialParameters, Real backgroundTransparency)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* renderEngine = GetSubsystem<RenderEngine>();

	gfxDevice->SetViewport(viewport_);
	gfxDevice->SetRenderTarget(renderSurface_);
	gfxDevice->SetDepthStencil(nullptr);
	gfxDevice->SetDepthTestMode(COMPARISON_ALWAYS);
	gfxDevice->SetDepthWrite(false);
	gfxDevice->SetCullMode(CULL_NONE);

	if (!vertexShader_ && !pixelShader_)
	{
		auto* cache = GetSubsystem<ResourceCache>();

		if (webKitRendering_)
		{
			auto* vertexShaderCode = cache->GetResource<ShaderCode>("Shader/WebKit/v2f_c4f_t2f_t2f_d28f.hlsl");
			vertexShader_ = vertexShaderCode->GetShader(VS, {});

			auto* pixelShaderCode = cache->GetResource<ShaderCode>("Shader/WebKit/fill.hlsl");
			pixelShader_ = pixelShaderCode->GetShader(PS, {});
		}
		else
		{
			auto* vertexShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
			vertexShader_ = vertexShaderCode->GetShader(VS, {});

			auto* pixelShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
			pixelShader_ = pixelShaderCode->GetShader(PS, {});
		}
	}

	if (webKitRendering_)
	{
		gfxDevice->SetBlendMode(BLEND_ALPHA);
	}
	else
	{
		gfxDevice->SetBlendMode(BLEND_REPLACE);
	}

	auto& shaderParameters = renderEngine->GetShaderParameters();
	shaderParameters.SetValue(SP_WORLD_MATRIX, Matrix3x4(Vector3(-1.0f, 1.0f), Quaternion::IDENTITY, Vector3(2.0f / viewport_.Width(), -2.0f / viewport_.Height(), 1.0f)));
	gfxDevice->SetEngineShaderParameters(&shaderParameters);
	gfxDevice->SetMaterialShaderParameters(materialParameters);

	gfxDevice->ClearVertexBuffer();
	gfxDevice->AddVertexBuffer(vertexBuffer_->GetGfxRef());
	gfxDevice->SetVertexDescription(vertexBuffer_->GetVertexDescription());

	gfxDevice->SetShaders(vertexShader_->GetGfxRef(), pixelShader_->GetGfxRef());

	UInt32 vertexStart = 0;

	for (UInt32 i = 0; i < batches_.Size(); ++i)
	{
		const auto& batch = batches_[i];

		materialParameters->SetValue<Real>("transparency", i == 0 && webKitRendering_ ? 0.0f : 1.0f);

		if (auto* texture = batch->GetTexture())
		{
			gfxDevice->SetTexture(0, texture->GetGfxTextureRef());
			gfxDevice->SetSampler(0, texture->GetGfxSamplerRef());
		}
		else
		{
			gfxDevice->SetTexture(0, defaultTexture->GetGfxTextureRef());
			gfxDevice->SetSampler(0, defaultTexture->GetGfxSamplerRef());
		}

		switch (batch->GetType())
		{
		case DRAW_LINE:
			gfxDevice->SetPrimitiveType(PRIMITIVE_LINE);
			break;

		case DRAW_TRIANGLE:
			gfxDevice->SetPrimitiveType(PRIMITIVE_TRIANGLE);
			break;
		}

		gfxDevice->Draw(vertexStart, batch->GetVertexCount());
		vertexStart += batch->GetVertexCount();
	}
}

}
