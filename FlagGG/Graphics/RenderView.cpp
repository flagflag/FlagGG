#include "RenderView.h"
#include "Graphics/Viewport.h"
#include "Graphics/RenderContext.h"
#include "Graphics/RenderPipline.h"
#include "Graphics/RenderEngine.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Scene/Octree.h"
#include "Scene/Light.h"
#include "Scene/Probe.h"
#include "ParticleSystem/ParticleSystemRenderer.h"
#include "Math/Polyhedron.h"
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
	, renderEngine_(GetSubsystem<RenderEngine>())
{
	shadowCameraNode_ = new Node();
	shadowCamera_ = shadowCameraNode_->CreateComponent<Camera>();
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
	octree_ = scene_ ? scene_->GetComponent<Octree>() : nullptr;
	viewport_ = viewport->GetSize();
	renderPiplineContext_ = nullptr;

	if (!renderPipline_)
	{
		if (!defaultRenderPipline_)
			defaultRenderPipline_ = new ForwardRenderPipline();
		renderPipline_ = defaultRenderPipline_;
	}

	needRenderView_ = scene_ && camera_ && renderPipline_;
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
	if (!needRenderView_)
		return;

	renderPiplineContext_ = &renderPipline_->GetRenderPiplineContext();
	renderPiplineContext_->Clear();

	renderPiplineContext_->renderSolution_ = IntVector2(viewport_.Width(), viewport_.Height());
	renderPiplineContext_->renderTarget_ = renderTarget_;
	renderPiplineContext_->depthStencil_ = depthStencil_;
	renderPiplineContext_->camera_ = camera_;

	if (auto* particleSystemRenderer = scene_->GetComponent<ParticleSystemRenderer>())
	{
		particleSystemRenderer->RenderUpdate(renderPiplineContext_);
	}

	CollectVisibilityObjects();

	renderPipline_->Clear();
	renderPipline_->CollectBatch();
}

void RenderView::CollectVisibilityObjects()
{
	tempQueryResults_.Clear();
	FrustumOctreeQuery query(tempQueryResults_, camera_->GetFrustum(), DRAWABLE_ANY, camera_->GetViewMask());
	octree_->GetElements(query);

	for (auto* drawableComponent : tempQueryResults_)
	{
		if (auto* light = drawableComponent->Cast<Light>())
		{
			renderPiplineContext_->lights_.Push(light);
		}
		else if (auto* probe = drawableComponent->Cast<Probe>())
		{
			renderPiplineContext_->probes_.Push(probe);
		}
		else
		{
			renderPiplineContext_->drawables_.Push(drawableComponent);
		}
	}

	if (renderPipline_->RenderShadowMap())
	{
// 找到第一个平行光
		Light* dirLight = nullptr;
		for (auto* light : renderPiplineContext_->lights_)
		{
			if (light->GetLightType() == LIGHT_TYPE_DIRECTIONAL)
			{
				dirLight = light;
				break;
			}
		}

		if (!dirLight)
			return;

// 设置平行光阴影相机参数
		const bool nonUniform = true;
		const float quantize = 0.5f;

		Node* lightNode = dirLight->GetNode();
		const float extrusionDistance = 10.f;
		
		const Quaternion lightRotation = lightNode->GetWorldRotation();
		// 阴影深度是在Y轴朝上的坐标系下计算的
		// Look-Z, Right-X, Up-Z
		const Vector3 pos = camera_->GetNode()->GetWorldPosition() - extrusionDistance * (lightRotation * Vector3::UP);
		shadowCameraNode_->SetTransform(pos, lightRotation, Vector3::ONE);

		Frustum splitFrustum = camera_->GetFrustum();
		Polyhedron frustumVolume;
		frustumVolume.Define(splitFrustum);

		// 裁剪视口内可见物体
		{
			BoundingBox litGeometriesBox;

			for (auto* drawable : tempQueryResults_)
			{
				if (drawable->GetCastShadows())
					litGeometriesBox.Merge(drawable->GetWorldBoundingBox());
			}

			if (litGeometriesBox.Defined())
			{
				frustumVolume.Clip(litGeometriesBox);
				// If volume became empty, restore it to avoid zero size
				if (frustumVolume.Empty())
					frustumVolume.Define(splitFrustum);
			}
		}

		const Matrix3x4& lightView = shadowCamera_->GetViewMatrix();
		frustumVolume.Transform(lightView);

		BoundingBox shadowBox;
		if (!nonUniform)
			shadowBox.Define(Sphere(frustumVolume));
		else
			shadowBox.Define(frustumVolume);

		shadowCamera_->SetOrthographic(true);
		shadowCamera_->SetAspect(1.f);
		shadowCamera_->SetNearClip(0.f);
		shadowCamera_->SetFarClip(shadowBox.max_.z_);

		const float minX = shadowBox.min_.x_;
		const float minY = shadowBox.min_.y_;
		const float maxX = shadowBox.max_.x_;
		const float maxY = shadowBox.max_.y_;

		Vector2 center((minX + maxX) * 0.5f, (minY + maxY) * 0.5f);
		Vector2 viewSize(maxX - minX, maxY - minY);

		if (nonUniform)
		{
			viewSize.x_ = ceilf(sqrtf(viewSize.x_ / quantize));
			viewSize.y_ = ceilf(sqrtf(viewSize.y_ / quantize));
			viewSize.x_ = Max(viewSize.x_ * viewSize.x_ * quantize, 3.f);
			viewSize.y_ = Max(viewSize.y_ * viewSize.y_ * quantize, 3.f);
		}
		else
		{
			viewSize.x_ = Max(viewSize.x_, viewSize.y_);
			viewSize.x_ = ceilf(sqrtf(viewSize.x_ / quantize));
			viewSize.x_ = Max(viewSize.x_ * viewSize.x_ * quantize, 3.f);
			viewSize.y_ = viewSize.x_;
		}

		shadowCamera_->SetOrthoSize(viewSize);

		Quaternion rot(shadowCameraNode_->GetWorldRotation());
		Vector3 adjust(center.x_, center.y_, 0.f);
		shadowCameraNode_->SetWorldPosition(shadowCameraNode_->GetWorldPosition() + rot * adjust);

// 找到被平行光视锥体裁剪的物件
		const Frustum& shadowCameraFrustum = shadowCamera_->GetFrustum();
		tempQueryResults_.Clear();
		ShadowCasterOctreeQuery query(tempQueryResults_, shadowCameraFrustum, DRAWABLE_GEOMETRY, camera_->GetViewMask());
		octree_->GetElements(query);

// 收集投射阴影的物件
		renderPiplineContext_->shadowCamera_ = shadowCamera_;
		renderPiplineContext_->shadowLight_ = dirLight;
		for (auto* shadowCaster : tempQueryResults_)
		{
			if (shadowCaster->GetCastShadows())
				renderPiplineContext_->shadowCasters_.Push(shadowCaster);
		}
	}
}

void RenderView::Render()
{
	if (!needRenderView_)
		return;

	float aspect = (float)viewport_.Width() / viewport_.Height();
	camera_->SetAspect(aspect);

	gfxDevice_->SetViewport(viewport_);

	renderPipline_->PrepareRender();

	renderPipline_->Render();
}

void RenderView::HandleEndFrame(Real timeStep)
{
	Undefine();
}

}
