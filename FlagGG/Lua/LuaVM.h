#ifndef __LUA_VM__
#define __LUA_VM__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/List.h"
#include "Container/Vector.h"
#include "Lua/ILua/LuaType.h"
#include "Lua/ILua/StackGet.h"
#include "Lua/ILua/StackSet.h"

#include <stdint.h>
#include <lua.hpp>

struct lua_State;

namespace FlagGG
{

class FlagGG_API LuaVM : public RefCounted
{
public:
	LuaVM();

	~LuaVM() override;

	operator lua_State*()
	{
		return luaState_;
	}

	// 打开虚拟机
	void Open();

	// 关闭虚拟机
	void Close();

	bool IsOpen() const;

	// 调用lua接口
	template < class ... Args >
	bool CallEvent(const String& eventName, Args&& ... args)
	{
		StackGuard guard(luaState_);

		return Call(luaState_, eventName, std::forward<Args>(args)...);
	}

	/**
	* @brief 注册通用函数接口
	* @param librarys : 函数代理集合
	*/
	void RegisterCEvents(const Vector<LuaProxy>& librarys);

	/**
		* @brief 注册类成员函数接口
		* @param className : 在lua中的表名
		* @param instance : 类实例
		* @param librarys : 类成员函数代理的集合
		*/
	void RegisterCPPEvents(const String& className, void* instance, const Vector<LuaProxy>& librarys);

	// 获取lua栈index位置的值
	template < class T >
	T Get(int index = -1)
	{
		return FlagGG::Get<T>(luaState_, index);
	}

	// 特化LuaFunction版本
	template <>
	LuaFunction Get<LuaFunction>(int index)
	{
		LuaFunction func = FlagGG::Get<LuaFunction>(luaState_, index);
		func.luaState_ = callHistory_.Front();
		return func;
	}

	// push值到lua栈顶
	template < class T >
	void Set(const T& value)
	{
		FlagGG::Set(luaState_, value);
	}

	// 从路径读取lua脚本并且执行
	bool Execute(const String& filePath);

	// 执行lua脚本
	bool ExecuteScript(const String& scriptCode);

	// 设置lua加载路径
	void SetLoaderPath(const String& loaderPath);

	friend class LuaVMGuard;

private:
	lua_State* luaState_;

	List<lua_State*> callHistory_;
};

}

#endif
