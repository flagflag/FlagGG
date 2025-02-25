#include "GameBuilder.h"
#include "GameBuilder/Brush/Brush.h"
#include "GameBuilder/Brush/TerrainBrushComponent.h"
#include "GameBuilder/Brush/LightBrushComponent.h"
#include "GameBuilder/Brush/TextureBrushComponent.h"

#include <Lua/LuaVM.h>
#include <Lua/LuaBinding/LuaBinding.h>
#include <Lua/LuaBinding/LuaExtend.h>

GameBuilder::GameBuilder()
{
	
}

GameBuilder::~GameBuilder()
{

}

void GameBuilder::Setup(Scene* scene)
{
	brush_ = new Brush(scene);

	auto brushComp = MakeShared<TextureBrushComponent>();
	brush_->AttachComponent(brushComp);

	ExportLuaAPI();
}

int GameBuilder::GetBrush(LuaVM* luaVM)
{
	luaex_pushusertyperef(*luaVM, "Brush", brush_);
	return 1;
}

void GameBuilder::ExportLuaAPI()
{
	LuaVM* luaVM = GetSubsystem<Context>()->GetVariable<LuaVM>("LuaVM");
	lua_State* L = *luaVM;

	luaex_beginclass(L, "Brush", "", nullptr, [](lua_State* L) -> int
	{
		if (auto* brush = reinterpret_cast<Brush*>(luaex_tousertype(L, 1, "Brush")))
		{
			brush->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "get_component", [](lua_State* L) -> int
		{
			if (auto* brush = reinterpret_cast<Brush*>(luaex_tousertype(L, 1, "Brush")))
			{
				const char* compName = lua_tostring(L, 2);
				auto* brushComp = brush->GetComponent(compName);
				luaex_pushusertyperef(L, compName, brushComp);
			}
			return 1;
		});
	}
	luaex_endclass(L);

	luaex_beginclass(L, "BrushComponent", "", nullptr, [](lua_State* L) -> int
	{
		if (auto* brushComp = reinterpret_cast<BrushComponent*>(luaex_tousertype(L, 1, "BrushComponent")))
		{
			brushComp->ReleaseRef();
		}
		return 0;
	});
	luaex_endclass(L);

	luaex_beginclass(L, "TextureBrushComponent", "BrushComponent", nullptr, [](lua_State* L) -> int
	{
		if (auto* brushComp = reinterpret_cast<TextureBrushComponent*>(luaex_tousertype(L, 1, "TextureBrushComponent")))
		{
			brushComp->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "set_brush_id", [](lua_State* L) -> int
		{
			if (auto* brushComp = reinterpret_cast<TextureBrushComponent*>(luaex_tousertype(L, 1, "TextureBrushComponent")))
			{
				brushComp->SetBrushId(lua_tointeger(L, 2));
			}
			return 0;
		});
	}
	luaex_endclass(L);

	luaVM->RegisterCPPEvents(
		"game_builder",
		this,
		{
			LUA_API_PROXY(GameBuilder, GetBrush, "get_brush"),
		});
}
