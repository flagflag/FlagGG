#include "LuaInterface/LuaGame.h"
#include "Unit/Unit.h"

#include <Scene/Scene.h>
#include <Scene/PrefabLoader.h>
#include <Lua/LuaBinding/LuaBinding.h>
#include <Lua/LuaBinding/LuaExtend.h>

LuaGamePlay::LuaGamePlay(Scene* scene)
	: scene_(scene)
{
	luaVM_ = GetSubsystem<Context>()->GetVariable<LuaVM>("LuaVM");
	luaVM_->RegisterCPPEvents(
		"gameplay",
		this,
		{
			LUA_API_PROXY(LuaGamePlay, Login, "login"),
			LUA_API_PROXY(LuaGamePlay, StartGame, "start_game"),
			LUA_API_PROXY(LuaGamePlay, EndGame, "end_game"),
			LUA_API_PROXY(LuaGamePlay, GetScene, "get_scene")
		}
	);

	gameplay_ = GetSubsystem<Context>()->GetVariable<GamePlayBase>("GamePlayBase");

	lua_State* L = *luaVM_;

	luaex_beginclass(L, "Node", "",
		[](lua_State* L) -> int
	{
		Node* node = new Node();
		luaex_pushusertyperef(L, "Node", node);
		return 1;
	},
		[](lua_State* L) -> int

	{
		if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
		{
			node->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "set_position", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				const Vector3 position(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
				node->SetPosition(position);
			}
			return 0;
		});
		luaex_classfunction(L, "set_scale", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				const Vector3 scale(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
				node->SetScale(scale);
			}
			return 0;
		});
	}
	luaex_endclass(L);

	luaex_function(L, "LoadPrefab", [](lua_State* L) -> int
	{
		const char* assetPath = lua_tostring(L, 1);
		auto node = LoadPrefab(assetPath);
		luaex_pushusertyperef(L, "Node", node);
		return 1;
	});

	luaex_beginclass(L, "Scene", "Node", nullptr, [](lua_State* L) -> int
	{
		if (auto* scene = reinterpret_cast<Scene*>(luaex_tousertype(L, 1, "Scene")))
		{
			scene->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "add_child", [](lua_State* L) -> int
		{
			if (auto* scene = reinterpret_cast<Scene*>(luaex_tousertype(L, 1, "Scene")))
			{
				auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 2, "Node"));
				if (node)
				{
					scene->AddChild(node);
				}
			}
			return 0;
		});
	}
	luaex_endclass(L);

	luaex_beginclass(L, "ClientUnit", "Node",
		[](lua_State* L) -> int
	{
		Unit* unit = new Unit();
		luaex_pushusertyperef(L, "ClientUnit", unit);
		return 1;
	},
		[](lua_State* L) -> int
	{
		if (auto* unit = reinterpret_cast<Unit*>(luaex_tousertype(L, 1, "ClientUnit")))
		{
			unit->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "load", [](lua_State* L) -> int
		{
			bool ret = false;
			if (auto* unit = reinterpret_cast<Unit*>(luaex_tousertype(L, 1, "ClientUnit")))
			{
				const char* assetPath = lua_tostring(L, 2);
				ret = unit->Load(assetPath);
			}
			lua_pushboolean(L, ret);
			return 1;
		});
	}
	luaex_endclass(L);
}

int LuaGamePlay::GetScene(LuaVM* luaVM)
{
	luaex_pushusertyperef(*luaVM, "Scene", scene_);
	return 1;
}

int LuaGamePlay::Login(LuaVM* luaVM)
{
	gameplay_->Login(luaVM->Get<LuaFunction>(1));
	return 0;
}

int LuaGamePlay::StartGame(LuaVM* luaVM)
{
	gameplay_->StartGame();
	return 0;
}

int LuaGamePlay::EndGame(LuaVM* luaVM)
{
	gameplay_->EndGame();
	return 0;
}

