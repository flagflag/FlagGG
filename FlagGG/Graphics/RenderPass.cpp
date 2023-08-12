#include "RenderPass.h"
#include "Scene/DrawableComponent.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Node.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Material.h"
#include "GfxDevice/GfxDevice.h"

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
		it.second_.renderBatchQueue_.renderBatches_.Clear();
		it.second_.renderBatchQueue_.renderBatchGroups_.Clear();
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

	auto& renderBatches = it->second_.renderBatchQueue_.renderBatches_;

	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_SHADOW);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_SHADOW;
			renderBatch.vertexShader_ = it->second_.vertexShader_;
			renderBatch.pixelShader_ = it->second_.pixelShader_;
		}
	}
}

void ShadowRenderPass::SortBatch()
{

}

void ShadowRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = RenderEngine::Instance();

	for (auto& it : shadowRenderContextMap_)
	{
		auto& renderBatches = it.second_.renderBatchQueue_.renderBatches_;
		for (auto& renderBatch : renderBatches)
		{
			renderEngine->SetRasterizerState(/*rasterizerState_*/renderBatch.material_->GetRasterizerState());
			renderEngine->SetDepthStencilState(depthStencilState_);
			renderEngine->SetShaderParameter(shadowCamera, renderBatch);
			renderEngine->SetShaders(renderBatch.vertexShader_, renderBatch.pixelShader_);
			renderEngine->SetVertexBuffers(renderBatch.geometry_->GetVertexBuffers());
			renderEngine->SetIndexBuffer(renderBatch.geometry_->GetIndexBuffer());
			renderEngine->SetPrimitiveType(renderBatch.geometry_->GetPrimitiveType());
			renderEngine->DrawCallIndexed(renderBatch.geometry_->GetIndexStart(), renderBatch.geometry_->GetIndexCount());
		}
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
		it.second_.renderBatchQueue_.renderBatchGroups_.Clear();
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

	auto& renderBatches = it->second_.renderBatchQueue_.renderBatches_;

	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		auto& renderBatch = renderBatches.EmplaceBack(renderContext);
		renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_LIT;
	}
}

void LitRenderPass::SortBatch()
{

}

void LitRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = RenderEngine::Instance();

	for (auto& it : litRenderContextMap_)
	{
		Node* lightCameraNode = shadowCamera->GetNode();

		auto& engineShaderParameters = renderEngine->GetShaderParameters();
		engineShaderParameters.SetValue(SP_LIGHT_POS, lightCameraNode->GetWorldPosition());
		engineShaderParameters.SetValue(SP_LIGHT_DIR, lightCameraNode->GetWorldRotation() * Vector3::FORWARD);
		engineShaderParameters.SetValue(SP_LIGHT_VIEW_MATRIX, shadowCamera->GetViewMatrix());
		engineShaderParameters.SetValue(SP_LIGHT_PROJVIEW_MATRIX, shadowCamera->GetProjectionMatrix() * shadowCamera->GetViewMatrix());
		auto shadowMap = renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP);
		if (shadowMap)
		{
			engineShaderParameters.SetValue(SP_SHADOWMAP_PIXEL_TEXELS, Vector2(1.f / shadowMap->GetWidth(), 1.f / shadowMap->GetHeight()));
		}

		auto& renderBatches = it.second_.renderBatchQueue_.renderBatches_;
		for (auto& renderBatch : renderBatches)
		{
			renderEngine->DrawBatch(camera, renderBatch);
		}
	}
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

}

void AlphaRenderPass::CollectBatch(RenderPassContext* context)
{

}

void AlphaRenderPass::SortBatch()
{

}

void AlphaRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{

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
	renderBatchQueue_.renderBatchGroups_.Clear();
}

void DeferredBaseRenderPass::CollectBatch(RenderPassContext* context)
{
	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_DEFERRED_BASE);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_SHADOW;
			renderBatch.vertexShader_ = it->second_.vertexShader_;
			renderBatch.pixelShader_ = it->second_.pixelShader_;
		}
	}
}

void DeferredBaseRenderPass::SortBatch()
{

}

void DeferredBaseRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = RenderEngine::Instance();

	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (auto& renderBatch : renderBatches)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}
}

}
