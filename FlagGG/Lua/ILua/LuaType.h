#ifndef __LUA_TYPE__
#define __LUA_TYPE__

#include "Container/Str.h"
#include "Lua/ILua/LuaUtil.h"

#include <stdint.h>
#include <lua.hpp>

struct lua_State;

namespace FlagGG
{
	namespace Lua
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

			LuaFunction(lua_State* luaState, int32_t idx);

			LuaFunction(const LuaFunction& func);

			virtual ~LuaFunction();

			LuaFunction& operator=(const LuaFunction& func);

			template < int returnCount = 0, class ... Args >
			void operator()(Args ... args)
			{
				StackGuard guard(luaState_);

				lua_rawgeti(luaState_, LUA_REGISTRYINDEX, ref_);
				SetParam(luaState_, args ...);
				CallImpl(luaState_, sizeof...(args), returnCount);
			}

			friend class LuaVM;

		protected:
			void Release();

		private:
			lua_State* luaState_;

			int32_t ref_;
			int32_t* refCount_;
		};

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
		struct LuaTypeOf<Container::String> : std::integral_constant<Type::_, Type::STRING> {};
	}
}

#endif
