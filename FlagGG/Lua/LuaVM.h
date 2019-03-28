#ifndef __LUA_VM__
#define __LUA_VM__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/Vector.h"
#include "Lua/ILua/StackCore.h"
#include "Lua/ILua/LuaUtil.h"

#include <stdint.h>
#include <lua.hpp>

struct lua_State;

namespace FlagGG
{
	namespace Lua
	{
		class FlagGG_API LuaVM : public Container::RefCounted
		{
		public:
			LuaVM();

			~LuaVM() override;

			void Open();

			void Close();

			bool IsOpen() const;

			template < class ... Args >
			bool CallEvent(const Container::String& eventName, const Args ... args)
			{
				StackGuard guard(luaState_);

				return Call(luaState_, eventName, args ...);
			}

			void RegisterCEvent(const Container::String& eventName, const LuaProxy& library);

			void RegisterCPPEvent(const Container::String& className, void* instance, const Container::Vector<LuaProxy>& librarys);

			template < class T >
			const T& Get(int index = -1)
			{
				return FlagGG::Lua::Get<T>(luaState_, index);
			}

			template < class T >
			void Set(const T& value)
			{
				FlagGG::Lua::Set(luaState_, value);
			}

			bool Execute(const Container::String& filePath);

		private:
			lua_State* luaState_;
		};
	}
}

#endif
