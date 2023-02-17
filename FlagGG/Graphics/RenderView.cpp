#include "RenderView.h"
#include "Graphics/Viewport.h"
#include "Graphics/RenderContext.h"
#include "Graphics/RenderPipline.h"
#include "Graphics/RenderEngine.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Scene/Octree.h"
#include "Scene/Light.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"

namespace FlagGG
{

// 阴影投射者OctreeQuery
class ShadowCasterOctreeQuery : public FrustumOctreeQuery
{
public:
    ShadowCasterOctreeQuery(PODVector<DrawableComponent*>& result, const Frustum& frustum, UInt8 drawableFlags = DRAWABLE_ANY,
        UInt32 viewMask = DEFAULT_VIEWMASK) :
        FrustumOctreeQuery(result, frustum, drawableFlags, viewMask)
    {
    }

    void TestDrawables(DrawableComponent** start, DrawableComponent** end, bool inside) override
    {
        while (start != end)
        {
			DrawableComponent* drawable = *start++;

            if (drawable->GetCastShadows() && (drawable->GetDrawableFlags() & drawableFlags_) &&
                (drawable->GetViewMask() & viewMask_))
            {
                if (inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
                    result_.Push(drawable);
            }
        }
    }
};

RenderView::RenderView()
	: gfxDevice_(GfxDevice::GetDevice())
	, renderEngine_(RenderEngine::Instance())
{
#if !OCTREE_QUERY
	shadowRasterizerState_.depthWrite_ = true;
	shadowRasterizerState_.scissorTest_ = false;
	shadowRasterizerState_.fillMode_ = FILL_SOLID;
	shadowRasterizerState_.cullMode_ = CULL_FRONT;
#endif
}

RenderView::~RenderView()
{

}

void RenderView::Define(Viewport* viewport)
{
	renderPipline_ = viewport->GetRenderPipline();
	renderTarget_ = viewport->GetRenderTarget();
	depthStencil_ = viewport->GetDepthStencil();
	scene_ = viewport->GetScene();
	camera_ = viewport->GetCamera();
	octree_ = scene_->GetComponent<Octree>();
	viewport_ = viewport->GetSize();
	renderPiplineContext_ = nullptr;

	if (!renderPipline_)
	{
		if (!defaultRenderPipline_)
			defaultRenderPipline_ = new ForwardRenderPipline();
		renderPipline_ = defaultRenderPipline_;
	}
}

void RenderView::Undefine()
{
	renderPipline_.Reset();
	scene_.Reset();
	camera_.Reset();
	octree_.Reset();
	renderPiplineContext_ = nullptr;
}

void RenderView::RenderUpdate()
{
#if OCTREE_QUERY
	renderPiplineContext_ = &renderPipline_->GetRenderPiplineContext();
	renderPiplineContext_->Clear();

	renderPiplineContext_->renderTarget_ = renderTarget_;
	renderPiplineContext_->depthStencil_ = depthStencil_;
	renderPiplineContext_->camera_ = camera_;

	CollectVisibilityObjects();

	renderPipline_->Clear();
	renderPipline_->CollectBatch();
#else
	visibleRenderContext_.Clear();
	visibleLights_.Clear();

	scene_->Render(visibleRenderContext_);
	scene_->GetLights(visibleLights_);
#endif
}

void RenderView::CollectVisibilityObjects()
{
#if OCTREE_QUERY
	FrustumOctreeQuery query(tempQueryResults_, camera_->GetFrustum(), DRAWABLE_ANY, camera_->GetViewMask());
	octree_->GetElements(query);

	for (auto* drawableComponent : tempQueryResults_)
	{
		if (auto* light = drawableComponent->DynamicCast<Light>())
		{
			renderPiplineContext_->lights_.Push(light);
			// temp
			light->SetAspect((float)viewport_.Width() / viewport_.Height());
		}
		//else if (auto* probe = drawableComponent->DynamicCast<Probe>())
		//{
		//	renderPiplineContext_->probes_.Push(probe);
		//}
		else
		{
			renderPiplineContext_->drawables_.Push(drawableComponent);
		}
	}
#endif
}

void RenderView::RenderShadowMap()
{

}

void RenderView::Render()
{
#if OCTREE_QUERY
	if (!scene_ || !camera_ || !renderPipline_)
		return;

	float aspect = (float)viewport_.Width() / viewport_.Height();
	camera_->SetAspect(aspect);

	gfxDevice_->SetViewport(viewport_);

	renderPipline_->Render();
#else
	if (!scene_ || !camera_ || !renderPipline_)
		return;

	gfxDevice_->SetViewport(viewport_);

	float aspect = (float)viewport_.Width() / viewport_.Height();

	// 反射相机不处理阴影
	if (!camera_->GetUseReflection() && SetShadowMap())
	{
		for (auto light : visibleLights_)
		{
			for (const auto& renderContext : visibleRenderContext_)
			{
				if ((renderContext->viewMask_ & camera_->GetViewMask()) != renderContext->viewMask_)
					continue;
				if (!renderContext->material_)
					continue;
				auto it = renderContext->material_->GetRenderPass().Find(RENDER_PASS_TYPE_SHADOW);
				if (it != renderContext->material_->GetRenderPass().End())
				{
					light->SetAspect(aspect);
					renderEngine_->SetRasterizerState(shadowRasterizerState_);
					renderEngine_->SetShaderParameter(light, renderContext);
					renderEngine_->SetShaders(it->second_.vertexShader_, it->second_.pixelShader_);
					renderEngine_->SetVertexBuffers(renderContext->geometry_->GetVertexBuffers());
					renderEngine_->SetIndexBuffer(renderContext->geometry_->GetIndexBuffer());
					renderEngine_->SetPrimitiveType(renderContext->geometry_->GetPrimitiveType());
					renderEngine_->DrawCallIndexed(renderContext->geometry_->GetIndexStart(), renderContext->geometry_->GetIndexCount());
				}
			}
		}
	}

	gfxDevice_->SetRenderTarget(renderTarget_);
	gfxDevice_->SetDepthStencil(depthStencil_);
	gfxDevice_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);

	for (const auto& renderContext : visibleRenderContext_)
	{
		if ((renderContext->viewMask_ & camera_->GetViewMask()) != renderContext->viewMask_)
			continue;

		if (visibleLights_.Size() > 0)
		{
			Node* lightNode = visibleLights_[0]->GetNode();
			visibleLights_[0]->SetAspect(aspect);
			auto& engineShaderParameters = renderEngine_->GetShaderParameters();
			engineShaderParameters.SetValue(SP_LIGHT_POS, lightNode->GetWorldPosition());
			engineShaderParameters.SetValue(SP_LIGHT_DIR, lightNode->GetWorldRotation() * Vector3::FORWARD);
			engineShaderParameters.SetValue(SP_LIGHT_VIEW_MATRIX, visibleLights_[0]->GetViewMatrix());
			engineShaderParameters.SetValue(SP_LIGHT_PROJVIEW_MATRIX, visibleLights_[0]->GetProjectionMatrix() * visibleLights_[0]->GetViewMatrix());
		}

		camera_->SetAspect(aspect);
		renderEngine_->SetRasterizerState(renderContext->material_->GetRasterizerState());
		renderEngine_->SetShaderParameter(camera_, renderContext);
		renderEngine_->SetShaders(renderContext->material_->GetVertexShader(), renderContext->material_->GetPixelShader());
		renderEngine_->SetMaterialTextures(renderContext->material_);
		renderEngine_->SetVertexBuffers(renderContext->geometry_->GetVertexBuffers());
		renderEngine_->SetIndexBuffer(renderContext->geometry_->GetIndexBuffer());
		renderEngine_->SetPrimitiveType(renderContext->geometry_->GetPrimitiveType());
		renderEngine_->DrawCallIndexed(renderContext->geometry_->GetIndexStart(), renderContext->geometry_->GetIndexCount());
}
#endif
}

void RenderView::HandleEndFrame(Real timeStep)
{
	Undefine();

#if !OCTREE_QUERY
	visibleRenderContext_.Clear();
#endif
}

#if !OCTREE_QUERY
bool RenderView::SetShadowMap()
{
	if (renderEngine_->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP))
	{
		gfxDevice_->SetRenderTarget(renderEngine_->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP)->GetRenderSurface());
		gfxDevice_->SetDepthStencil(nullptr);

		gfxDevice_->Clear(CLEAR_COLOR, Color::WHITE);

		return true;
	}

	return false;
}
#endif

}
