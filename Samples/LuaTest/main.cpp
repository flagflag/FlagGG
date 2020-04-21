#include <Lua/LuaVM.h>
#include <Core/Profiler.h>
#include <Math/Vector3.h>
#include <Log.h>

#include <string>

class LuaLog
{
public:
	int Normal(FlagGG::Lua::LuaVM* vm)
	{
		printf("%s\n", vm->Get<const char*>(1));
		return 0;
	}
};

FlagGG::Core::ProfilerBlock proBlock(nullptr, "Test");

static int ProfilerBegin(lua_State *L)
{
	proBlock.Begin();
	return 0;
}

static int ProfilerEnd(lua_State *L)
{
	proBlock.End();
	return 0;
}

static int OutputProfiler(lua_State* L)
{
	proBlock.EndFrame();

	printf("profiler cost: %lld.\n", proBlock.frameTime_);
	return 0;
}

FlagGG::Math::Vector3 parseVector3(const std::string& val, const FlagGG::Math::Vector3& defaultValue = FlagGG::Math::Vector3::ZERO)
{
	const int Dimension = 3;
	const char* begin = val.c_str();
	char* end;
	float value[Dimension];
	for (int i = 0; i < Dimension; i++)
	{
		value[i] = (float)strtod(begin, &end);
		if (begin == end)
			return defaultValue;
		begin = end;
	}
	return FlagGG::Math::Vector3(value[0], value[1], value[2]);
}

void castTypeFromString(FlagGG::Math::Vector3 &arg, const std::string& str)
{
	arg = parseVector3(str);
}

template< typename T >
struct EngineUnmarshallData
{
	T operator()(const char* str) const
	{
		T value;
		castTypeFromString(value, str);
		return value;
	}
};

template<typename T>
T getStruct(lua_State* L, Int32 index)
{
	return EngineUnmarshallData<T>()(lua_tostring(L, index));
}

FlagGG::Math::Vector3 GetVector3(lua_State* L, int index)
{
	int len = lua_rawlen(L, index);
	if (len < 3)
		return FlagGG::Math::Vector3::ZERO;

	FlagGG::Math::Vector3 vec3;

	lua_rawgeti(L, index, 1);
	vec3.x_ = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, index, 2);
	vec3.y_ = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, index, 3);
	vec3.z_ = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return vec3;
}

static int Test1(lua_State* L)
{
	FlagGG::Math::Vector3 param1 = getStruct<FlagGG::Math::Vector3>(L, -1);
	return 0;
}

static int Test2(lua_State* L)
{
	FlagGG::Math::Vector3 param1 = GetVector3(L, 1);
	return 0;
}

void Run()
{
	FlagGG::Utility::SystemHelper::HiresTimer::InitSupported();

	enum
	{
		_2333 = 0
	};

	FlagGG::Lua::LuaVM luaVM;
	luaVM.Open();
	if (!luaVM.IsOpen())
	{
		FLAGGG_LOG_ERROR("open lua vm failed.");

		return;
	}

	LuaLog logInstance;

	luaVM.RegisterCPPEvents(
		"log",
		&logInstance,
		{
			LUA_API_PROXY(LuaLog, Normal, "normal")
		}
	);

	luaVM.RegisterCEvents(
	{
		C_LUA_API_PROXY(Test1, "Test1"),
		C_LUA_API_PROXY(Test2, "Test2"),
		C_LUA_API_PROXY(ProfilerBegin, "ProfilerBegin"),
		C_LUA_API_PROXY(ProfilerEnd, "ProfilerEnd"),
		C_LUA_API_PROXY(OutputProfiler, "OutputProfiler"),
	});

	luaVM.ExecuteScript(R"(
ProfilerBegin()
for i = 1, 100000 do
	Test1("0.23 0.65 0.77")
end
ProfilerEnd()
OutputProfiler()

local temp = {}
local function Vector3(x, y, z)
	temp[1] = x
	temp[2] = y
	temp[3] = z
	return temp
end

ProfilerBegin()
for i = 1, 100000 do
	Test2(Vector3(0.23, 0.65, 0.77))
end
ProfilerEnd()
OutputProfiler()


Test = {}
function Test.normal(...)
	log.normal(...)
end
)");

	luaVM.CallEvent("Test.normal", true);
	luaVM.CallEvent("Test.normal", _2333);
	luaVM.CallEvent("Test.normal", (int)233);
	luaVM.CallEvent("Test.normal", (float)6666.0);
	luaVM.CallEvent("Test.normal", (double)6666.0);
	luaVM.CallEvent("Test.normal", "2333");
	const char* temp0 = "2333";
	luaVM.CallEvent("Test.normal", temp0);
	luaVM.CallEvent("Test.normal", FlagGG::Container::String("2333"));
	FlagGG::Container::String temp1("2333");
	luaVM.CallEvent("Test.normal", temp1);
	const FlagGG::Container::String temp2("2333");
	luaVM.CallEvent("Test.normal", temp2);
}

int main()
{
	Run();

	getchar();

	return 0;
}