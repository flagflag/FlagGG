#include "Core/CryAssert.h"
#include "Container/Str.h"
#include "Log.h"

#include "LuaExtend.h"

namespace FlagGG
{

lua_State* luaex_getmainstate(lua_State* L)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
	lua_State* mainState = lua_tothread(L, -1);
	lua_pop(L, 1);
	return mainState;
}

void luaex_pushglobaltable(lua_State* L, const char* name, bool keyWeakRef/* = false*/, bool valueWeakRef/* = false*/)
{
	lua_pushstring(L, name);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		if (keyWeakRef || valueWeakRef)
		{
			lua_newtable(L);
			lua_pushliteral(L, "__mode");
			if (keyWeakRef && !valueWeakRef)
				lua_pushliteral(L, "k");
			else if (!keyWeakRef && valueWeakRef)
				lua_pushliteral(L, "v");
			else
				lua_pushliteral(L, "kv");
			lua_rawset(L, -3);
			lua_setmetatable(L, -2);
		}
		lua_pushstring(L, name);
		lua_pushvalue(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}
}

void luaex_pushcontext(lua_State* L)
{
	luaex_pushglobaltable(L, "LuaexContext");
	ASSERT_MESSAGE(lua_istable(L, -1), "LuaexContext is not a table");
}

static void luaex_pushusertypetable(lua_State* L)
{
	luaex_pushglobaltable(L, "LuaexUserData", false, true);
	ASSERT_MESSAGE(lua_istable(L, -1), "LuaexUserData is not a table");
}

bool luaex_juedge_inherit(lua_State* L, const String& type, const String& superClass)
{
	// 拿出继承关系
	luaex_pushcontext(L);
	lua_pushliteral(L, "ClassMap");
	lua_rawget(L, -2);
	ASSERT(lua_istable(L, -1));

	String subClass = type;
	bool findIt = false;
	while (!subClass.Empty())
	{
		if (subClass == superClass)
		{
			findIt = true;
			lua_pop(L, 1);
			break;
		}

		lua_pushstring(L, subClass.CString());
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			break;
		}
		subClass = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	lua_pop(L, 1);
	return findIt;
}

int luaex_pushusertype(lua_State* L, const char* userType, void* userValue)
{
	if (!userValue)
	{
		lua_pushnil(L);
		return 0;
	}

	luaex_pushusertypetable(L);

	// 获取UserTable
	lua_pushlightuserdata(L, userValue);
	lua_rawget(L, -2);

	if (lua_istable(L, -1))
	{
		lua_pushliteral(L, "__cname");
		lua_rawget(L, -2);
		const char* name = lua_tostring(L, -1);
		lua_pop(L, 1);

		// 判断类型一致，或者存在继承关系
		if (strcmp(name, userType) == 0 || luaex_juedge_inherit(L, name, userType) || luaex_juedge_inherit(L, userType, name))
		{
#ifdef _DEBUG
			// check 一下
			lua_pushliteral(L, "__cinstance");
			lua_rawget(L, -2);
			void* cinstance = lua_touserdata(L, -1);
			ASSERT(!cinstance || cinstance == userValue);
			lua_pop(L, 1);
#endif
			// 移除userdata table
			lua_remove(L, -2);
			return 0;
		}

		// 这种情况因该是指针地址重用了，这时候lua保存的指针在C++中已经析构，直接设置成空指针就好了，之后覆盖原有的userdata
		FLAGGG_LOG_STD_WARN("Export lua: address multi used, ptr:[%p] last type: [%s] this type:[%s]",
			userValue, name, userType);
		lua_pushliteral(L, "__cinstance");
		lua_pushlightuserdata(L, nullptr);
		lua_rawset(L, -3);
	}

	lua_pop(L, 1);
	luaex_pushcontext(L);
	// 新建UserTable
	lua_newtable(L);  // @newtable
	// 获取class表
	lua_pushstring(L, userType);
	lua_rawget(L, -3);
	ASSERT(lua_istable(L, -1));

	lua_getmetatable(L, -1);  // -1 is luaContext[userType]
	lua_setmetatable(L, -3);  // -3 is @newtable, setmetatable(@newtable, luaContext[userType])

	lua_pop(L, 1);  // pop luaContext[userType]
	lua_remove(L, -2); // remove luaContext

	lua_pushliteral(L, "__cname");
	lua_pushstring(L, userType);
	lua_rawset(L, -3);  // @newtable["__cname"] = userType

	lua_pushliteral(L, "__cinstance");
	lua_pushlightuserdata(L, userValue);
	lua_rawset(L, -3);  // @newtable["__cinstance"] = &userValue

	lua_pushlightuserdata(L, userValue);
	lua_pushvalue(L, -2);  // push @newtable
	lua_rawset(L, -4);     // LuaexUserData[&userValue] = @newtable

	// 移除userdata table
	lua_remove(L, -2);  // remove LuaexUserData

	// return @newtable
	return 1;
}

int luaex_pushusertyperef(lua_State* L, const char* userType, RefCounted* ref)
{
	int ret = luaex_pushusertype(L, userType, ref);
	if (ret != 0)
	{
		ref->AddRef();
	}
	return ret;
}

void* luaex_tousertype(lua_State* L, int idx, const char* userType)
{
#ifdef _DEBUG
	// check一下
	lua_pushliteral(L, "__cname");
	lua_rawget(L, idx > 0 ? idx : idx - 1);
	String type = lua_tostring(L, -1);
	lua_pop(L, 1);

	if (!luaex_juedge_inherit(L, type, userType))
		return nullptr;
#endif
	lua_pushliteral(L, "__cinstance");
	lua_rawget(L, idx > 0 ? idx : idx - 1);
	void* userValue = lua_touserdata(L, -1);
	lua_pop(L, 1);
	return userValue;
}

}
