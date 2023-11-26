#include "Core/CryAssert.h"
#include "Container/Str.h"
#include "Container/Vector.h"

#include "LuaBinding.h"
#include "LuaExtend.h"

#include <cstring>

namespace FlagGG
{

static void luaex_record_class(lua_State* L, const char* name, const char* base)
{
	// 记录一下继承关系
	lua_pushliteral(L, "ClassMap");
	lua_rawget(L, -2);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushliteral(L, "ClassMap");
		lua_pushvalue(L, -2);
		lua_rawset(L, -4);
	}
	lua_pushstring(L, name);
	lua_pushstring(L, base);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

void luaex_beginclass(lua_State* L, const char* name, const char* base, lua_CFunction contructor, lua_CFunction destructor)
{
	// 获取上下文
	luaex_pushcontext(L);

	luaex_record_class(L, name, base);

	// 创建class表，然后把他push到context里
	lua_newtable(L);  // @class_t
	lua_pushstring(L, name);
	lua_pushvalue(L, -2);
	lua_rawset(L, -4);

	// 设置class名
	lua_pushliteral(L, "__cname");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	// 创建一张空表用来保存元表
	lua_createtable(L, 1, 0);  // @meta_T

	// gc元表
	if (destructor)
	{
		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, destructor);
		lua_rawset(L, -3);
	}

	// index元表，后续通过luaex_classfunction往元表里插入class的function
	lua_pushliteral(L, "__index");

	lua_newtable(L);  // @index_t

	// 继承，把父类元表方法拷过来
	if (base && strlen(base) > 0)
	{
		luaex_pushcontext(L);
		lua_pushstring(L, base);
		lua_rawget(L, -2);
		ASSERT_MESSAGE(lua_istable(L, -1), "base class is not a table");

		lua_getmetatable(L, -1);
		lua_pushliteral(L, "__index");
		lua_rawget(L, -2);

		// 遍历元素，然后拷贝到另一张表中，不要直接用__index，因为是引用关系！！！
		Vector<String> keys;
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			keys.Push(lua_tostring(L, -2));
			lua_pop(L, 1);
		}
		for (const auto& key : keys)
		{
			lua_pushstring(L, key.CString());
			lua_rawget(L, -2);

			lua_pushstring(L, key.CString());
			lua_pushvalue(L, -2);
			lua_rawset(L, -8);
			lua_pop(L, 1);
		}

		lua_pop(L, 4);
	}

	if (contructor)
	{
		luaex_classfunction(L, "__create", contructor);

		// 同时把构造函数引用一份作为new函数调用
		lua_pushliteral(L, "new");
		lua_pushcfunction(L, contructor);
		lua_rawset(L, -6);
	}
}

void luaex_endclass(lua_State* L)
{
	// 把classfunction表插入元表
	lua_rawset(L, -3);

	// 设置为class表的元表
	lua_setmetatable(L, -2);

	// 设置class类型为type == 1
	lua_pushliteral(L, "__ctype");
	lua_pushinteger(L, 1);
	lua_rawset(L, -3);

	// 复原堆栈
	lua_pop(L, 2);
}

void luaex_classfunction(lua_State* L, const char* name, lua_CFunction func)
{
	lua_pushstring(L, name);
	lua_pushcfunction(L, func);
	lua_rawset(L, -3);  // -3 is @index_t
}

void luaex_function(lua_State* L, const char* name, lua_CFunction func)
{
	luaex_pushcontext(L);
	lua_pushstring(L, name);
	lua_pushcfunction(L, func);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

void luaex_globalfunction(lua_State* L, const char* name, lua_CFunction func)
{
	lua_pushcfunction(L, func);
	lua_setglobal(L, name);
}

void luaex_constant(lua_State* L, const char* name, long long value)
{
	luaex_pushcontext(L);
	lua_pushstring(L, name);
	lua_pushinteger(L, value);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

void luaex_constant(lua_State* L, const char* name, const char* value)
{
	luaex_pushcontext(L);
	lua_pushstring(L, name);
	lua_pushstring(L, value);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

void luaex_classconstant(lua_State* L, const char* name, long long value)
{
	lua_pushstring(L, name);
	lua_pushinteger(L, value);
	lua_rawset(L, -3);
}

void luaex_classconstant(lua_State* L, const char* name, const String& value)
{
	lua_pushstring(L, name);
	lua_pushstring(L, value.CString());
	lua_rawset(L, -3);
}

}
