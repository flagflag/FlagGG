#include "LuaInterface/LuaGame.h"
#include "Unit/Unit.h"
#include "Unit/Terrain.h"
#include "Map/MapBuilder.h"

#include <Scene/Scene.h>
#include <Scene/PrefabLoader.h>
#include <Scene/Light.h>
#include <Scene/Probe.h>
#include <Lua/LuaBinding/LuaBinding.h>
#include <Lua/LuaBinding/LuaExtend.h>
#include <Lua/LuaBinding/LuaMath.h>

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
		luaex_classfunction(L, "set_name", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				node->SetName(lua_tostring(L, 2));
			}
			return 0;
		});
		luaex_classfunction(L, "set_position", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				const Vector3 position(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
				node->SetPosition(position);
			}
			return 0;
		});
		luaex_classfunction(L, "set_rotation", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				const Quaternion rotation(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
				node->SetRotation(rotation);
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
		luaex_classfunction(L, "get_child", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				const char* name = lua_tostring(L, 2);
				auto child = node->GetChild(StringHash(name), lua_toboolean(L, 3));
				luaex_pushusertyperef(L, "Node", child);
				return 1;
			}
			return 0;
		});
		luaex_classfunction(L, "get_component", [](lua_State* L) -> int
		{
			if (auto* node = reinterpret_cast<Node*>(luaex_tousertype(L, 1, "Node")))
			{
				const char* componentType = lua_tostring(L, 2);
				auto* comp = node->GetComponent(StringHash(componentType));
				luaex_pushusertyperef(L, comp->GetTypeName().CString(), comp);
				return 1;
			}
			return 0;
		});
	}
	luaex_endclass(L);

	luaex_beginclass(L, "Component", "", nullptr, [](lua_State* L) -> int
	{
		if (auto* comp = reinterpret_cast<Component*>(luaex_tousertype(L, 1, "Component")))
		{
			comp->ReleaseRef();
		}
		return 0;
	});
	luaex_endclass(L);

	luaex_beginclass(L, "Light", "Component", nullptr, [](lua_State* L) -> int
	{
		if (auto* light = reinterpret_cast<Light*>(luaex_tousertype(L, 1, "Light")))
		{
			light->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "set_brightness", [](lua_State* L) -> int
		{
			if (auto* light = reinterpret_cast<Light*>(luaex_tousertype(L, 1, "Light")))
			{
				light->SetBrightness(lua_tonumber(L, 2));
			}
			return 0;
		});
	}
	luaex_endclass(L);

	luaex_beginclass(L, "Probe", "Component", nullptr, [](lua_State* L) -> int
	{
		if (auto* probe = reinterpret_cast<Probe*>(luaex_tousertype(L, 1, "Probe")))
		{
			probe->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "load", [](lua_State* L) -> int
		{
			if (auto* probe = reinterpret_cast<Probe*>(luaex_tousertype(L, 1, "Probe")))
			{
				probe->Load(lua_tostring(L, 2));
			}
			return 0;
		});
		luaex_classfunction(L, "set_diffuse_intensity", [](lua_State* L) -> int
		{
			if (auto* probe = reinterpret_cast<Probe*>(luaex_tousertype(L, 1, "Probe")))
			{
				probe->SetDiffuseIntensity(lua_tonumber(L, 2));
			}
			return 0;
		});
		luaex_classfunction(L, "set_specular_intensity", [](lua_State* L) -> int
		{
			if (auto* probe = reinterpret_cast<Probe*>(luaex_tousertype(L, 1, "Probe")))
			{
				probe->SetSpecularIntensity(lua_tonumber(L, 2));
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

	luaex_beginclass(L, "Terrain", "Node",
		[](lua_State* L) -> int
	{
		Terrain* terrain = new Terrain();
		luaex_pushusertyperef(L, "Terrain", terrain);
		return 1;
	},
		[](lua_State* L) -> int
	{
		if (auto* terrain = reinterpret_cast<Terrain*>(luaex_tousertype(L, 1, "Terrain")))
		{
			terrain->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "create", [](lua_State* L) -> int
		{
			if (auto* terrain = reinterpret_cast<Terrain*>(luaex_tousertype(L, 1, "Terrain")))
			{
				UInt32 patchSize = lua_tointeger(L, 2);
				Vector3 quadSize = luaex_tovector3(L, 3);
				const char* heightMapPath = lua_tostring(L, 4);
				const char* matPath = lua_tostring(L, 5);
				terrain->Create(patchSize, quadSize, heightMapPath, matPath);
			}
			return 0;
		});
	}
	luaex_endclass(L);

	luaex_beginclass(L, "MapBuilder", "",
		[](lua_State* L)->int
	{
		SharedPtr<MapBuilder> mapBuilder(new MapBuilder());
		luaex_pushusertyperef(L, "MapBuilder", mapBuilder);
		return 1;
	}, [](lua_State* L)->int
	{
		if (auto* mapBuilder = reinterpret_cast<MapBuilder*>(luaex_tousertype(L, 1, "MapBuilder")))
		{
			mapBuilder->ReleaseRef();
		}
		return 0;
	});
	{
		luaex_classfunction(L, "set_scene", [](lua_State* L)->int
		{
			if (auto* mapBuilder = reinterpret_cast<MapBuilder*>(luaex_tousertype(L, 1, "MapBuilder")))
			{
				auto* scene = reinterpret_cast<Scene*>(luaex_tousertype(L, 2, "Scene"));
				mapBuilder->SetScene(scene);
			}
			return 0;
		});
		luaex_classfunction(L, "load_map", [](lua_State* L)->int
		{
			if (auto* mapBuilder = reinterpret_cast<MapBuilder*>(luaex_tousertype(L, 1, "MapBuilder")))
			{
				const char* path = lua_tostring(L, 2);
				mapBuilder->LoadMap(path);
			}
			return 0;
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

