#include "RenderPipline.h"
#include "Graphics/RenderPass.h"
#include "Scene/Light.h"
#include "Scene/DrawableComponent.h"
#include "Lua/ILua/LuaType.h"

namespace FlagGG
{

void RenderPiplineContext::Clear()
{
	renderTarget_ = nullptr;
	depthStencil_ = nullptr;
	camera_ = nullptr;
	drawables_.Clear();
	lights_.Clear();
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
	, litRenderPass_{ SharedPtr<RenderPass>(new LitRenderPass()), SharedPtr<RenderPass>(new LitRenderPass()) }
	, alphaRenderPass_(new AlphaRenderPass())
{

}

CommonRenderPipline::~CommonRenderPipline()
{

}

void CommonRenderPipline::Clear()
{
	shadowRenderPass_->Clear();
	litRenderPass_[0]->Clear();
	litRenderPass_[1]->Clear();
	alphaRenderPass_->Clear();
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
		auto& litRenderObjects = litRenderObjectsResult_.EmplaceBack();

		litRenderObjects.light_ = light;

		auto type = light->GetLightType();
		switch (type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			for(auto* drawable : renderPiplineContext_.drawables_)
			{
				litRenderObjects.drawables_.Push(drawable);
			}
			break;

		case LIGHT_TYPE_POINT:

			break;

		case LIGHT_TYPE_SPOT:

			break;
		}
	}

	for (auto* drawable : renderPiplineContext_.drawables_)
	{
		drawable->SetHasLitPass(false);
	}

	RenderPassContext context;

// 生成受光batch
	for (auto& litRenderObjects : litRenderObjectsResult_)
	{
		context.light_ = litRenderObjects.light_;
		for (auto* drawable : litRenderObjects.drawables_)
		{
			context.drawable_ = drawable;

			// shadow pass
			shadowRenderPass_->CollectBatch(&context);

			// litbase
			if (!drawable->GetHasLitPass())
			{
				drawable->SetHasLitPass(true);
				litRenderPass_[0]->CollectBatch(&context);
			}
			// light
			else
			{
				litRenderPass_[1]->CollectBatch(&context);
			}
		}
	}
}

void CommonRenderPipline::CollectUnlitBatch()
{

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
