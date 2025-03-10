#include "RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ComputePass.h"
#include "Scene/Light.h"
#include "Scene/DrawableComponent.h"
#include "Lua/ILua/LuaType.h"
#include "Core/EngineSettings.h"

namespace FlagGG
{

void RenderPiplineContext::Clear()
{
	renderTarget_ = nullptr;
	depthStencil_ = nullptr;
	camera_ = nullptr;
	drawables_.Clear();
	shadowCasters_.Clear();
	lights_.Clear();
	probes_.Clear();
	shadowLight_ = nullptr;
	probes_.Clear();
}

void LitRenderObjects::Clear()
{
	light_ = nullptr;
	drawables_.Clear();
}

/*********************************************************/
/*                     RenderPipline                     */
/*********************************************************/

RenderPipline::RenderPipline()
{
}

RenderPipline::~RenderPipline()
{

}

/*********************************************************/
/*                  CommonRenderPipline                  */
/*********************************************************/

CommonRenderPipline::CommonRenderPipline()
	: shadowRenderPass_(new ShadowRenderPass())
	, alphaRenderPass_(new AlphaRenderPass())
	, waterRenderPass_(new WaterRenderPass())
{
	if (GetSubsystem<EngineSettings>()->clusterLightType_ != ClusterLightType::None)
		clusterLightPass_ = new ClusterLightPass();
}

CommonRenderPipline::~CommonRenderPipline()
{

}

void CommonRenderPipline::Clear()
{
	shadowRenderPass_->Clear();
	alphaRenderPass_->Clear();
}

void CommonRenderPipline::CollectBatch()
{
	GpuOcclusionCulling();

	CollectLitBatch();

	CollectUnlitBatch();
}

void CommonRenderPipline::CollectLitBatch()
{
// 筛选出"灯+受光"的物体
	litRenderObjectsResult_.Clear();

	Sort(renderPiplineContext_.lights_.Begin(), renderPiplineContext_.lights_.End(), [](Light* _1, Light* _2)
	{
		return _1->GetLightType() < _2->GetLightType();
	});

	for (auto* light : renderPiplineContext_.lights_)
	{
		if (light->GetLightType() == LIGHT_TYPE_DIRECTIONAL)
		{
			auto& litRenderObjects = litRenderObjectsResult_.EmplaceBack();
			litRenderObjects.light_ = light;

			for (auto* drawable : renderPiplineContext_.drawables_)
			{
				litRenderObjects.drawables_.Push(drawable);
			}
		}
	}

// 生成受光batch
	OnSolveLitBatch();
}

void CommonRenderPipline::CollectUnlitBatch()
{
	RenderPassContext context{};

	for (auto& drawable : renderPiplineContext_.drawables_)
	{
		context.light_ = nullptr;
		context.drawable_ = drawable;
		alphaRenderPass_->CollectBatch(&context);
	}
}

/*********************************************************/
/*                  ScriptRenderPipline                  */
/*********************************************************/

ScriptRenderPipline::ScriptRenderPipline(lua_State* state)
	: state_(state)
{

}

ScriptRenderPipline::~ScriptRenderPipline()
{

}

void ScriptRenderPipline::CollectBatch()
{
	UserTypeRef userTypeRef(GetTypeName(), this, state_);
	userTypeRef.Call("CollectBatch");
}

void ScriptRenderPipline::PrepareRender()
{
	UserTypeRef userTypeRef(GetTypeName(), this, state_);
	userTypeRef.Call("PrepareRender");
}

void ScriptRenderPipline::Render()
{
	UserTypeRef userTypeRef(GetTypeName(), this, state_);
	userTypeRef.Call("Render");
}

}
