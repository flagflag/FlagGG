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

	void BuildVertexBuffer();

	void Render(Texture2D* defaultTexture);
};

UISystem::UISystem()
{
	defaultTexture_ = new Texture2D();
	defaultTexture_->SetNumLevels(1u);
	defaultTexture_->SetSize(1, 1, TEXTURE_FORMAT_RGBA8);
	UInt32 colorValue = Color::WHITE.ToUInt();
	defaultTexture_->SetData(0, 0, 0, 1, 1, &colorValue);

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
		batchRenderData.batches_.Clear();
		batchRenderData.renderSurface_ = it.renderSurface_;
		batchRenderData.viewport_ = it.viewport_;
		batchRenderData.manualRender_ = it.manualRender_;

		vertexVector_.Clear();

		UpdateBatches(it.uiRoot_, &vertexVector_, batchRenderData.batches_);

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

	batchRenderData->Render(defaultTexture_);
}

void UIBatchRenderData::BuildVertexBuffer()
{
	if (!vertexBuffer_)
	{
		vertexBuffer_ = new VertexBuffer();
	}

	static PODVector<VertexElement> vertexElement =
	{
		VertexElement(VE_VECTOR3, SEM_POSITION, 0),
		VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),
		VertexElement(VE_UBYTE4_UNORM, SEM_COLOR, 0)
	};

	UInt32 vertexCount = 0u;

	for (const auto& batch : batches_)
	{
		vertexCount += batch->GetVertexCount();
	}

	vertexBuffer_->SetSize(vertexCount, vertexElement);
	char* data = (char*)vertexBuffer_->Lock(0, vertexBuffer_->GetVertexCount());

	for (const auto& batch : batches_)
	{
		Memory::Memcpy(data, batch->GetVertexs(), batch->GetVertexCount() * batch->GetVertexSize());
		data += (batch->GetVertexCount() * vertexBuffer_->GetVertexSize());
	}

	vertexBuffer_->Unlock();
}

void UIBatchRenderData::Render(Texture2D* defaultTexture)
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

		auto* vertexShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
		vertexShader_ = vertexShaderCode->GetShader(VS, {});

		auto* pixelShaderCode = cache->GetResource<ShaderCode>("Shader/UI.hlsl");
		pixelShader_ = pixelShaderCode->GetShader(PS, {});
	}

	auto& shaderParameters = renderEngine->GetShaderParameters();
	shaderParameters.SetValue(SP_WORLD_MATRIX, Matrix3x4(Vector3(-1.0f, 1.0f), Quaternion::IDENTITY, Vector3(2.0f / viewport_.Width(), -2.0f / viewport_.Height(), 1.0f)));
	gfxDevice->SetEngineShaderParameters(&shaderParameters);

	gfxDevice->ClearVertexBuffer();
	gfxDevice->AddVertexBuffer(vertexBuffer_->GetGfxRef());
	gfxDevice->SetVertexDescription(vertexBuffer_->GetVertexDescription());

	gfxDevice->SetShaders(vertexShader_->GetGfxRef(), pixelShader_->GetGfxRef());

	UInt32 vertexStart = 0;

	for (const auto& batch : batches_)
	{
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
