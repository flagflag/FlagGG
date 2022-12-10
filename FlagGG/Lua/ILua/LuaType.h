#ifndef __LUA_TYPE__
#define __LUA_TYPE__

#include "Container/Str.h"
#include "Container/RefCounted.h"
#include "Lua/ILua/LuaUtil.h"
#include "Lua/LuaBinding/LuaExtend.h"
#include "Core/BaseTypes.h"

#include <lua.hpp>

struct lua_State;

namespace FlagGG
{

namespace Type
{
	enum
	{
		NONE = LUA_TNONE,
		NIL = LUA_TNIL,
		STRING = LUA_TSTRING,
		NUMBER = LUA_TNUMBER,
		THREAD = LUA_TTHREAD,
		BOOLEAN = LUA_TBOOLEAN,
		FUNCTION = LUA_TFUNCTION,
		USERDATA = LUA_TUSERDATA,
		LIGHT_USERDATA = LUA_TLIGHTUSERDATA,
		TABLE = LUA_TTABLE,
	};
	typedef int _;
}

class FlagGG_API LuaFunction
{
public:
	LuaFunction();

	LuaFunction(lua_State* luaState, Int32 idx);

	LuaFunction(const LuaFunction& func);

	virtual ~LuaFunction();

	LuaFunction& operator=(const LuaFunction& func);

	template < int returnCount = 0, class ... Args >
	void operator()(Args&& ... args)
	{
		StackGuard guard(luaState_);

		lua_rawgeti(luaState_, LUA_REGISTRYINDEX, ref_);
		SetParam(luaState_, std::forward<Args>(args)...);
		CallImpl(luaState_, sizeof...(args), returnCount);
	}

	friend class LuaVM;

protected:
	void Release();

private:
	lua_State* luaState_;

	Int32 ref_;
	Int32* refCount_;
};

class FlagGG_API UserTypeRef
{
public:
	UserTypeRef();

	UserTypeRef(const String& userType, void* userValue, lua_State* L = nullptr);

	UserTypeRef(const String& userType, RefCounted* userValue, lua_State* L = nullptr);

	UserTypeRef(const UserTypeRef& ref);

	UserTypeRef(UserTypeRef&& ref);

	UserTypeRef& operator=(const UserTypeRef&) = default;

	template < int returnCount = 0, class ... Args >
	void Call(const String& eventName, Args ... args);

	String userType_;
	void* userValue_;
	RefCounted* userValueRef_;
	lua_State* L_;
};

template <>
struct Setter<UserTypeRef>
{
	static void Set(lua_State* L, const UserTypeRef& userTypeRef)
	{
		if (userTypeRef.userValue_)
			luaex_pushusertype(L, userTypeRef.userType_.CString(), userTypeRef.userValue_);
		else
			luaex_pushusertyperef(L, userTypeRef.userType_.CString(), userTypeRef.userValueRef_);
	}
};

template < int returnCount, class ... Args >
void UserTypeRef::Call(const String& eventName, Args ... args)
{
	SetParam(L_, *this);
	lua_pushstring(L_, eventName.CString());
	lua_gettable(L_, -2);
	if (lua_isfunction(L_, -1) || lua_iscfunction(L_, -1))
	{
		SetParam(L_, std::forward<Args>(args)...);
		CallImpl(L_, sizeof...(args), returnCount);
		lua_pop(L_, 1);
	}
	else
	{
		lua_pop(L_, 2);
	}
}

inline Type::_ TypeOf(lua_State* L, int index) { return lua_type(L, index); }

template < class T, class = void >
struct LuaTypeOf : std::integral_constant<Type::_, Type::USERDATA> {};

template <>
struct LuaTypeOf<std::nullptr_t> : std::integral_constant<Type::_, Type::NIL> {};

template <>
struct LuaTypeOf<lua_CFunction> : std::integral_constant<Type::_, Type::FUNCTION> {};

template <>
struct LuaTypeOf<bool> : std::integral_constant<Type::_, Type::BOOLEAN> {};

template < class T >
struct LuaTypeOf<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> : std::integral_constant<Type::_, Type::NUMBER> {};

template < class T >
struct LuaTypeOf<T, typename std::enable_if<std::is_enum<T>::value>::type> : std::integral_constant<Type::_, Type::NUMBER> {};

template <>
struct LuaTypeOf<const char*> : std::integral_constant<Type::_, Type::STRING> {};

template <>
struct LuaTypeOf<String> : std::integral_constant<Type::_, Type::STRING> {};

}

#endif
