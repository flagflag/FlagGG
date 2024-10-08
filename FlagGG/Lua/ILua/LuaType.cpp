#include "Lua/ILua/LuaType.h"

namespace FlagGG
{

LuaFunction::LuaFunction() :
	luaState_(nullptr),
	ref_(0),
	refCount_(nullptr)
{ }

LuaFunction::LuaFunction(lua_State* luaState, Int32 idx) :
	luaState_(luaState),
	refCount_(new Int32{ 1 })
{
	lua_pushvalue(luaState_, idx);
	ref_ = luaL_ref(luaState_, LUA_REGISTRYINDEX);
}

LuaFunction::LuaFunction(const LuaFunction& func) :
	refCount_(nullptr)
{
	*(this) = func;
}

LuaFunction::~LuaFunction()
{
	Release();
}

LuaFunction& LuaFunction::operator=(const LuaFunction& func)
{
	Release();

	luaState_ = func.luaState_;
	ref_ = func.ref_;
	refCount_ = func.refCount_;
	if (refCount_)
		++(*refCount_);

	return (*this);
}

void LuaFunction::Release()
{
	if (refCount_)
	{
		--(*refCount_);
		if ((*refCount_) == 0)
		{
			delete refCount_;
			refCount_ = 0;

			if (ref_)
			{
				luaL_unref(luaState_, LUA_REGISTRYINDEX, ref_);
				ref_ = 0;
			}
		}
	}
}

UserTypeRef::UserTypeRef() :
	userValue_(nullptr),
	L_(nullptr)
{}

UserTypeRef::UserTypeRef(const String& userType, void* userValue, lua_State* L) :
	userType_(userType),
	userValue_(userValue),
	userValueRef_(nullptr),
	L_(L)
{

}

UserTypeRef::UserTypeRef(const String& userType, RefCounted* userValue, lua_State* L) :
	userType_(userType),
	userValue_(nullptr),
	userValueRef_(userValue),
	L_(L)
{

}

UserTypeRef::UserTypeRef(const UserTypeRef& ref)
{
	userType_ = ref.userType_;
	userValue_ = ref.userValue_;
	userValueRef_ = ref.userValueRef_;
	L_ = ref.L_;
}

UserTypeRef::UserTypeRef(UserTypeRef&& ref) :
	userType_(std::move(ref.userType_)),
	userValue_(ref.userValue_),
	userValueRef_(ref.userValueRef_),
	L_(ref.L_)
{

}

}

