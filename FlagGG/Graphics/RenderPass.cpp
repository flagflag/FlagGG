#include "RenderPass.h"
#include "Scene/DrawableComponent.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Probe.h"
#include "Scene/Node.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Material.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/ComputePass.h"
#include "Resource/Image.h"
#include "Resource/ResourceCache.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxTexture.h"
#include "Core/EngineSettings.h"
#include "Core/Profiler.h"

namespace FlagGG
{

/*********************************************************/
/*                       RenderPass                      */
/*********************************************************/

RenderPass::RenderPass()
{

}

RenderPass::~RenderPass()
{

}

/*********************************************************/
/*                    ShadowRenderPass                   */
/*********************************************************/

ShadowRenderPass::ShadowRenderPass()
{
	rasterizerState_.scissorTest_ = false;
	rasterizerState_.fillMode_ = FILL_SOLID;
	rasterizerState_.cullMode_ = CULL_FRONT;
	rasterizerState_.blendMode_ = BLEND_REPLACE;

	depthStencilState_.depthWrite_ = true;
	depthStencilState_.depthTestMode_ = COMPARISON_LESS_EQUAL;
	depthStencilState_.stencilTest_ = false;
}

ShadowRenderPass::~ShadowRenderPass()
{

}

void ShadowRenderPass::Clear()
{
	for (auto& it : shadowRenderContextMap_)
	{
		it.second_.renderBatchQueue_.Clear();
	}
}

void ShadowRenderPass::CollectBatch(RenderPassContext* context)
{
	auto it = shadowRenderContextMap_.Find(context->light_);
	if (it == shadowRenderContextMap_.End())
	{
		it = shadowRenderContextMap_.Insert(MakePair(context->light_, ShadowRenderContext()));
		it->second_.light_ = context->light_;
	}

	auto& renderBatchQueue = it->second_.renderBatchQueue_;

	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_SHADOW);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			FlagGG::RenderBatch renderBatch(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_SHADOW;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
			renderBatchQueue.AddBatch(renderBatch, true);
		}
	}
}

void ShadowRenderPass::SortBatch()
{
	PROFILE_AUTO(ShadowRenderPass::SortBatch);

	for (auto& it : shadowRenderContextMap_)
	{
		it.second_.renderBatchQueue_.SyncInstanceDataToGpu();
	}
}

void ShadowRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(ShadowRenderPass::RenderBatch);

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();
	auto* gfxDevice = GfxDevice::GetDevice();

	for (auto& it : shadowRenderContextMap_)
	{
		for (auto& renderBatch : it.second_.renderBatchQueue_.renderBatches_)
		{
			renderEngine->SetRasterizerState(renderBatch.renderPassInfo_->GetRasterizerState());
			gfxDevice->SetDepthBias(-0.25f, -5);
			renderEngine->SetDepthStencilState(depthStencilState_, false);
			renderEngine->SetShaderParameter(shadowCamera, renderBatch);
			renderEngine->SetShaders(renderBatch.vertexShader_, renderBatch.pixelShader_);
			renderEngine->SetVertexBuffers(renderBatch.geometry_->GetVertexBuffers());
			renderEngine->SetIndexBuffer(renderBatch.geometry_->GetIndexBuffer());
			renderEngine->SetPrimitiveType(renderBatch.geometry_->GetPrimitiveType());
			renderEngine->DrawCallIndexed(renderBatch.geometry_->GetIndexStart(), renderBatch.geometry_->GetIndexCount());
		}
		for (auto& renderBatch : it.second_.renderBatchQueue_.renderInstanceBatches_)
		{
			renderEngine->SetRasterizerState(renderBatch.renderPassInfo_->GetRasterizerState());
			gfxDevice->SetDepthBias(-0.25f, -5);
			renderEngine->SetDepthStencilState(depthStencilState_, false);
			renderEngine->SetShaderParameter(shadowCamera, renderBatch);
			renderEngine->SetShaders(renderBatch.vertexShader_, renderBatch.pixelShader_);
			renderEngine->SetVertexBuffers(renderBatch.geometry_->GetVertexBuffers());
			renderEngine->SetIndexBuffer(renderBatch.geometry_->GetIndexBuffer());
			renderEngine->SetPrimitiveType(renderBatch.geometry_->GetPrimitiveType());
			gfxDevice->SetInstanceBuffer(it.second_.renderBatchQueue_.instanceBuffer_->GetGfxRef());
			gfxDevice->SetInstanceDescription(it.second_.renderBatchQueue_.instanceBuffer_->GetVertexDescription());
			gfxDevice->DrawIndexedInstanced(renderBatch.geometry_->GetIndexStart(), renderBatch.geometry_->GetIndexCount(), 0, renderBatch.instanceStart_, renderBatch.instanceCount_);
		}
	}
}

bool ShadowRenderPass::HasAnyBatch() const
{
	for (auto& it : shadowRenderContextMap_)
	{
		if (it.second_.renderBatchQueue_.HasAnyBatch())
			return true;
	}

	return false;
}

/*********************************************************/
/*                     DepthRenderPass                   */
/*********************************************************/

DepthRenderPass::DepthRenderPass()
{

}

DepthRenderPass::~DepthRenderPass()
{

}

void DepthRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
}

void DepthRenderPass::CollectBatch(RenderPassContext* context)
{
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_DEPTH);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			FlagGG::RenderBatch renderBatch(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_DEPTH;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
			renderBatchQueue_.AddBatch(renderBatch, true);
		}
	}
}

void DepthRenderPass::SortBatch()
{
	PROFILE_AUTO(DepthRenderPass::SortBatch);

	renderBatchQueue_.SyncInstanceDataToGpu();
}

void DepthRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(DepthRenderPass::RenderBatch);

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	for (auto& renderBatch : renderBatchQueue_.renderBatches_)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}
	for (auto& renderBatch : renderBatchQueue_.renderInstanceBatches_)
	{
		renderEngine->DrawBatch(camera, renderBatch, renderBatchQueue_.instanceBuffer_);
	}
}

/*********************************************************/
/*                     LitRenderPass                     */
/*********************************************************/

LitRenderPass::LitRenderPass()
{
}

LitRenderPass::~LitRenderPass()
{

}

void LitRenderPass::Clear()
{
	for (auto& it : litRenderContextMap_)
	{
		it.second_.renderBatchQueue_.renderBatches_.Clear();
	}
}

void LitRenderPass::CollectBatch(RenderPassContext* context)
{
	auto it = litRenderContextMap_.Find(context->light_);
	if (it == litRenderContextMap_.End())
	{
		it = litRenderContextMap_.Insert(MakePair(context->light_, LitRenderContext()));
		it->second_.light_ = context->light_;
	}

	auto& renderBatchQueue = it->second_.renderBatchQueue_;

	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_FORWARD_LIT);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			FlagGG::RenderBatch renderBatch(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_LIT;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
			renderBatch.probe_ = context->probe_;
			renderBatchQueue.AddBatch(renderBatch, true);
		}
	}


}

void LitRenderPass::SortBatch()
{
	PROFILE_AUTO(LitRenderPass::SortBatch);

	for (auto& it : litRenderContextMap_)
	{
		it.second_.renderBatchQueue_.SyncInstanceDataToGpu();
	}
}

void LitRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(LitRenderPass::RenderBatch);

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	for (auto& it : litRenderContextMap_)
	{
		Light* light = it.first_;
		Node* lightCameraNode = light->GetNode();

		auto& engineShaderParameters = renderEngine->GetShaderParameters();
		engineShaderParameters.SetValue(SP_LIGHT_POS, lightCameraNode->GetWorldPosition());
		engineShaderParameters.SetValue(SP_LIGHT_DIR, -lightCameraNode->GetWorldDirection());
		engineShaderParameters.SetValue(SP_LIGHT_COLOR, light->GetEffectiveColor());
		engineShaderParameters.SetValue(SP_LIGHT_VIEW_MATRIX, shadowCamera->GetViewMatrix());
		engineShaderParameters.SetValue(SP_LIGHT_PROJVIEW_MATRIX, shadowCamera->GetProjectionMatrix() * shadowCamera->GetViewMatrix());
		auto shadowMap = renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP);
		if (shadowMap)
		{
			engineShaderParameters.SetValue(SP_SHADOWMAP_PIXEL_TEXELS, Vector2(1.f / shadowMap->GetWidth(), 1.f / shadowMap->GetHeight()));
		}

		Probe* probe = nullptr;

		for (auto& renderBatch : it.second_.renderBatchQueue_.renderBatches_)
		{
			if (renderBatch.probe_ != probe)
			{
				probe = renderBatch.probe_;
				if (probe)
					probe->ApplyRender(renderEngine);
			}

			renderEngine->DrawBatch(camera, renderBatch);
		}

		for (auto& renderBatch : it.second_.renderBatchQueue_.renderInstanceBatches_)
		{
			if (renderBatch.probe_ != probe)
			{
				probe = renderBatch.probe_;
				if (probe)
					probe->ApplyRender(renderEngine);
			}

			renderEngine->DrawBatch(camera, renderBatch, it.second_.renderBatchQueue_.instanceBuffer_);
		}
	}
}

bool LitRenderPass::HasAnyBatch() const
{
	for (auto& it : litRenderContextMap_)
	{
		if (it.second_.renderBatchQueue_.HasAnyBatch())
			return true;
	}

	return false;
}

/*********************************************************/
/*                    WaterRenderPass                    */
/*********************************************************/

WaterRenderPass::WaterRenderPass()
{

}

WaterRenderPass::~WaterRenderPass()
{

}

void WaterRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
}

void WaterRenderPass::CollectBatch(RenderPassContext* context)
{
	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_FORWARD_WATER);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_WATER;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
		}
	}
}

void WaterRenderPass::SortBatch()
{
	PROFILE_AUTO(WaterRenderPass::SortBatch);

	renderBatchQueue_.SyncInstanceDataToGpu();
}

#define DEBUG_WATER_DEPTH 0

void WaterRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(WaterRenderPass::RenderBatch);

	auto* gfxDevice = GfxDevice::GetDevice();
	auto* gfxRenderSurface = gfxDevice->GetRenderTarget(0);
	if (!gfxRenderSurface)
		return;
	auto* gfxRenderTexture = gfxRenderSurface->GetOwnerTexture();
	if (!gfxRenderTexture)
		return;
	auto* gfxDepthSurface = gfxDevice->GetDepthStencil();
	if (!gfxDepthSurface)
		return;
	auto* gfxDepthTexture = gfxDepthSurface->GetOwnerTexture();
	if (!gfxDepthTexture)
		return;

	if (!refractionTexture_)
	{
		refractionTexture_ = new Texture2D();
		screenDepthTexture_ = new Texture2D();
#if DEBUG_WATER_DEPTH
		debugTexture_ = new Texture2D();
#endif
	}

	const auto& rtDesc = gfxRenderTexture->GetDesc();
	if (rtDesc.width_ != refractionTexture_->GetWidth() ||
		rtDesc.height_ != refractionTexture_->GetHeight())
	{
		refractionTexture_->SetNumLevels(1);
		refractionTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
		refractionTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
		refractionTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
		refractionTexture_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		refractionTexture_->SetSize(rtDesc.width_, rtDesc.height_, rtDesc.format_, TEXTURE_DYNAMIC);

		screenDepthTexture_->SetNumLevels(1);
		screenDepthTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
		screenDepthTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
		screenDepthTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
		screenDepthTexture_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		screenDepthTexture_->SetSize(rtDesc.width_, rtDesc.height_, gfxDepthTexture->GetDesc().format_, TEXTURE_DYNAMIC);

#if DEBUG_WATER_DEPTH
		debugTexture_->SetNumLevels(1);
		debugTexture_->SetFilterMode(TEXTURE_FILTER_NEAREST);
		debugTexture_->SetSize(rtDesc.width_, rtDesc.height_, TEXTURE_FORMAT_RGBA32F, TEXTURE_RENDERTARGET);
#endif
	}

	auto* gfxRefractionTexture = refractionTexture_->GetGfxTextureRef();
	if (gfxRefractionTexture)
	{
		gfxRefractionTexture->UpdateTexture(gfxRenderTexture);
	}

	auto* gfxScreenDepthTexture = screenDepthTexture_->GetGfxTextureRef();
	if (gfxScreenDepthTexture)
	{
		gfxScreenDepthTexture->UpdateTexture(gfxDepthTexture);
	}

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();
	renderEngine->SetDefaultTexture(TextureClass(0), refractionTexture_);
	renderEngine->SetDefaultTexture(TextureClass(1), screenDepthTexture_);

#if DEBUG_WATER_DEPTH
	gfxDevice->SetRenderTarget(1, debugTexture_->GetRenderSurface());
#endif

	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (auto& renderBatch : renderBatches)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}

	renderEngine->SetDefaultTexture(TextureClass(0), nullptr);
	renderEngine->SetDefaultTexture(TextureClass(1), nullptr);

#if DEBUG_WATER_DEPTH
	gfxDevice->SetRenderTarget(1, nullptr);
#endif
}

/*********************************************************/
/*                    AlphaRenderPass                    */
/*********************************************************/

AlphaRenderPass::AlphaRenderPass()
{

}

AlphaRenderPass::~AlphaRenderPass()
{

}

void AlphaRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
}

void AlphaRenderPass::CollectBatch(RenderPassContext* context)
{
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_FORWARD_ALPHA);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			FlagGG::RenderBatch renderBatch(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_ALPHA;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
			renderBatchQueue_.AddBatch(renderBatch, true);
		}
	}


}

void AlphaRenderPass::SortBatch()
{
	PROFILE_AUTO(AlphaRenderPass::SortBatch);

	renderBatchQueue_.SyncInstanceDataToGpu();
}

void AlphaRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(AlphaRenderPass::RenderBatch);

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	for (auto& renderBatch : renderBatchQueue_.renderBatches_)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}

	for (auto& renderBatch : renderBatchQueue_.renderInstanceBatches_)
	{
		renderEngine->DrawBatch(camera, renderBatch, renderBatchQueue_.instanceBuffer_);
	}
}

/*********************************************************/
/*                 DeferredBaseRenderPass                */
/*********************************************************/

DeferredBaseRenderPass::DeferredBaseRenderPass()
{

}

DeferredBaseRenderPass::~DeferredBaseRenderPass()
{

}

void DeferredBaseRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
}

void DeferredBaseRenderPass::CollectBatch(RenderPassContext* context)
{
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_DEFERRED_BASE);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			FlagGG::RenderBatch renderBatch(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_DEFERRED_BASE;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
			renderBatchQueue_.AddBatch(renderBatch, true);
		}
	}
}

void DeferredBaseRenderPass::SortBatch()
{
	PROFILE_AUTO(DeferredBaseRenderPass::SortBatch);

	renderBatchQueue_.SyncInstanceDataToGpu();
}

void DeferredBaseRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(DeferredBaseRenderPass::RenderBatch);

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	for (auto& renderBatch : renderBatchQueue_.renderBatches_)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}

	for (auto& renderBatch : renderBatchQueue_.renderInstanceBatches_)
	{
		renderEngine->DrawBatch(camera, renderBatch, renderBatchQueue_.instanceBuffer_);
	}
}

/*********************************************************/
/*                  DeferredLitRenderPass                */
/*********************************************************/

DeferredLitRenderPass::DeferredLitRenderPass()
{

}

DeferredLitRenderPass::~DeferredLitRenderPass()
{

}

void DeferredLitRenderPass::Clear()
{

}

void DeferredLitRenderPass::CollectBatch(RenderPassContext* context)
{
	auto it = litRenderContextMap_.Find(context->light_);
	if (it == litRenderContextMap_.End())
	{
		it = litRenderContextMap_.Insert(MakePair(context->light_, DeferredLitRenderContext()));
		it->second_.light_ = context->light_;
		it->second_.probe_ = context->probe_;
	}
}

void DeferredLitRenderPass::SortBatch()
{

}

void DeferredLitRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	PROFILE_AUTO(DeferredLitRenderPass::RenderBatch);

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();
	GfxDevice* gfxDevice = GfxDevice::GetDevice();

	for (auto& it : litRenderContextMap_)
	{
		Light* light = it.first_;
		Node* lightCameraNode = light->GetNode();

		auto& engineShaderParameters = renderEngine->GetShaderParameters();
		engineShaderParameters.SetValue(SP_LIGHT_POS, lightCameraNode->GetWorldPosition());
		engineShaderParameters.SetValue(SP_LIGHT_DIR, -lightCameraNode->GetWorldDirection());
		engineShaderParameters.SetValue(SP_LIGHT_COLOR, light->GetEffectiveColor());
		engineShaderParameters.SetValue(SP_LIGHT_VIEW_MATRIX, shadowCamera->GetViewMatrix());
		engineShaderParameters.SetValue(SP_LIGHT_PROJVIEW_MATRIX, shadowCamera->GetProjectionMatrix() * shadowCamera->GetViewMatrix());
		auto shadowMap = renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP);
		if (shadowMap)
		{
			engineShaderParameters.SetValue(SP_SHADOWMAP_PIXEL_TEXELS, Vector2(1.f / shadowMap->GetWidth(), 1.f / shadowMap->GetHeight()));
			gfxDevice->SetTexture(TEXTURE_CLASS_SHADOWMAP, shadowMap->GetGfxTextureRef());
			gfxDevice->SetSampler(TEXTURE_CLASS_SHADOWMAP, shadowMap->GetGfxSamplerRef());
		}
		if (auto iblCube = renderEngine->GetDefaultTexture(TEXTURE_CLASS_IBL))
		{
			gfxDevice->SetTexture(TEXTURE_CLASS_IBL, iblCube->GetGfxTextureRef());
			gfxDevice->SetSampler(TEXTURE_CLASS_IBL, iblCube->GetGfxSamplerRef());
		}
		if (auto ssaoMap = renderEngine->GetDefaultTexture(TEXTURE_CLASS_SSAO))
		{
			gfxDevice->SetTexture(TEXTURE_CLASS_SSAO, ssaoMap->GetGfxTextureRef());
			gfxDevice->SetSampler(TEXTURE_CLASS_SSAO, ssaoMap->GetGfxSamplerRef());
		}

		if (!litVertexShader_ || !litPixelShader_)
		{
			auto shaderCode = GetSubsystem<ResourceCache>()->GetResource<ShaderCode>("Shader/Deferred/LitQuad.hlsl");

			auto vs = shaderCode->GetShader(VS, {});
			litVertexShader_ = vs->GetGfxRef();

			Vector<String> psDefines;

			if (GetSubsystem<EngineSettings>()->clusterLightEnabled_)
			{
				psDefines =
				{
					"DIRLIGHT",
					"AMBIENT",
					"DEFERRED_CLUSTER",
					ToString("SAMPLER_CLUSTERS_LIGHTINDICES=%d ", ClusterLightPass::GetRasterizerBinding(SAMPLER_CLUSTERS_LIGHTINDICES)),
					ToString("SAMPLER_CLUSTERS_LIGHTGRID=%d ", ClusterLightPass::GetRasterizerBinding(SAMPLER_CLUSTERS_LIGHTGRID)),
					ToString("SAMPLER_LIGHTS_POINTLIGHTS=%d ", ClusterLightPass::GetRasterizerBinding(SAMPLER_LIGHTS_POINTLIGHTS)),
				};
			}
			else
			{
				psDefines =
				{
					"DIRLIGHT",
					"AMBIENT",
				};
			}

			auto ps = shaderCode->GetShader(PS, psDefines);
			litPixelShader_ = ps->GetGfxRef();
		}

		gfxDevice->SetShaders(litVertexShader_, litPixelShader_);

		Probe* probe = it.second_.probe_;
		if (probe)
			probe->ApplyRender(renderEngine);

		renderEngine->DrawQuad(camera);
	}
}

bool DeferredLitRenderPass::HasAnyBatch() const
{
	return litRenderContextMap_.Size();
}

}
