#include <Lua/LuaVM.h>

int main()
{
	FlagGG::Lua::LuaVM luaVM;

	luaVM.CallEvent("2333", 233);

	return 0;
}