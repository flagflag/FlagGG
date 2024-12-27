#include <Lua/LuaVM.h>
#include <Lua/LuaBinding/LuaBinding.h>
#include <Lua/LuaBinding/LuaExtend.h>
#include <Core/Profiler.h>
#include <Math/Vector3.h>
#include <Container/RefCounted.h>
#include <Log.h>
#include <Memory/MemoryHook.h>

#include <string>

using namespace FlagGG;

class LuaLog
{
public:
	int Normal(LuaVM* vm)
	{
		printf("%s\n", vm->Get<const char*>(1));
		return 0;
	}

	int Alert(LuaVM* vm)
	{
#if _WIN32
		MessageBoxA(nullptr,  vm->Get<const char*>(1), "", 0);
#endif
		return 0;
	}
};

ProfilerBlock proBlock(nullptr, "Test");

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

Vector3 parseVector3(const std::string& val, const Vector3& defaultValue = Vector3::ZERO)
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
	return Vector3(value[0], value[1], value[2]);
}

void castTypeFromString(Vector3 &arg, const std::string& str)
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

Vector3 GetVector3(lua_State* L, int index)
{
	int len = lua_rawlen(L, index);
	if (len < 3)
		return Vector3::ZERO;

	Vector3 vec3;

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
	Vector3 param1 = getStruct<Vector3>(L, -1);
	return 0;
}

static int Test2(lua_State* L)
{
	Vector3 param1 = GetVector3(L, 1);
	return 0;
}

class TestBase : public RefCounted
{
public:
	TestBase()
	{
		FLAGGG_LOG_INFO("TestBase ctor");
	}

	~TestBase()
	{
		FLAGGG_LOG_INFO("TestBase dtor");
	}

	virtual void RunTest()
	{
		FLAGGG_LOG_INFO("TestBase::RunTest");
	}
};

static int luaex_TestBase_Ctor(lua_State* L)
{
	TestBase* cls = new TestBase();
	luaex_pushusertyperef(L, "TestBase", cls);
	return 1;
}

static int luaex_TestBase_Dtor(lua_State* L)
{
	TestBase* cls = (TestBase*)luaex_tousertype(L, 1, "TestBase");
	if (cls)
		cls->ReleaseRef();
	return 0;
}

static int luaex_TestBase_RunTest(lua_State* L)
{
	TestBase* cls = (TestBase*)luaex_tousertype(L, -1, "TestBase");
	cls->RunTest();
	return 0;
}

static int luaex_RunTest(lua_State* L)
{
	TestBase* cls = (TestBase*)luaex_tousertype(L, 1, "TestBase");
	UserTypeRef luaWrapper("TestBase", cls, L);
	luaWrapper.Call("RunTest2");
	return 0;
}

void Run()
{
	HiresTimer::InitSupported();

	enum
	{
		_2333 = 0
	};

	LuaVM luaVM;
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
			LUA_API_PROXY(LuaLog, Normal, "normal"),
			LUA_API_PROXY(LuaLog, Alert, "alert"),
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
	luaVM.CallEvent("Test.normal", String("2333"));
	String temp1("2333");
	luaVM.CallEvent("Test.normal", temp1);
	const String temp2("2333");
	luaVM.CallEvent("Test.normal", temp2);


	lua_State* L = luaVM;
	luaex_beginclass(L, "TestBase", "", luaex_TestBase_Ctor, luaex_TestBase_Dtor);
		luaex_classfunction(L, "RunTest", luaex_TestBase_RunTest);
	luaex_endclass(L);
	luaex_globalfunction(L, "RunTest", luaex_RunTest);

	luaVM.ExecuteScript(R"(
local SceneNode = class('SceneNode')

function SceneNode:ctor()

end

function SceneNode:RunTest()
	log.normal('SceneNode:RunTest')
end

function SceneNode:__gc()

end

local Context = GetContext()
local TestClass1 = class('TestClass1', Context.TestBase)

function TestClass1:ctor()
	log.normal('TestClass1:ctor')
end

function TestClass1:RunTest()
	Context.TestBase.RunTest(self)
	log.normal('TestClass::RunTest')
end

function TestClass1:RunTest2()
	log.normal('TestClass::RunTest2')
end

function TestClass1:__gc()
	log.normal('TestClass1:dtor')
end

_G.SceneNode = SceneNode
_G.TestClass1 = TestClass1

local test = TestClass1.new()
test:RunTest()

local test2 = SceneNode.new()
test2:RunTest()

RunTest(test)
)");
}

int main()
{
	Run();

	getchar();

	return 0;
}

IMPLEMENT_MODULE_USD("LuaTest");
