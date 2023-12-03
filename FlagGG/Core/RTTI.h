#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/StringHash.h"

namespace FlagGG
{

class FlagGG_API TypeInfo
{
public:
	TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo);
	~TypeInfo();

	bool IsTypeOf(StringHash type) const;
	bool IsTypeOf(const TypeInfo* typeInfo) const;
	template<typename T> bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

	StringHash GetType() const { return type_; }
	const String& GetTypeName() const { return typeName_; }
	const TypeInfo* GetBaseTypeInfo() const { return baseTypeInfo_; }

private:
	StringHash type_;
	String typeName_;
	const TypeInfo* baseTypeInfo_;
};

class FlagGG_API RTTIObject
{
public:
	virtual ~RTTIObject();

	virtual StringHash GetType() const = 0;
	virtual const String& GetTypeName() const = 0;
	virtual const TypeInfo* GetTypeInfo() const = 0;

	static const TypeInfo* GetTypeInfoStatic() { return nullptr; }
	bool IsInstanceOf(StringHash type) const;
	bool IsInstanceOf(const TypeInfo* typeInfo) const;
	template<typename T> bool IsInstanceOf() const { return IsInstanceOf(T::GetTypeInfoStatic()); }
	template<typename T> T* DynamicCast() { return IsInstanceOf<T>() ? static_cast<T*>(this) : nullptr; }
	template<typename T> const T* DynamicCast() const { return IsInstanceOf<T>() ? static_cast<const T*>(this) : nullptr; }
};

template <class T, class U>
inline T* Cast(U& object)
{
	return object ? object->DynamicCast<T>() : nullptr;
}

template <class T, class U>
inline T* Cast(U* object)
{
	return object ? object->DynamicCast<T>() : nullptr;
}

}

#define OBJECT_OVERRIDE(typeName, baseTypeName) \
	public: \
        using ClassName = typeName; \
        using BaseClassName = baseTypeName; \
		using SuperClassName = baseTypeName; \
        virtual FlagGG::StringHash GetType() const override { return GetTypeInfoStatic()->GetType(); } \
        virtual const FlagGG::String& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
        virtual const FlagGG::TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
        static FlagGG::StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
        static const FlagGG::String& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
        static const FlagGG::TypeInfo* GetTypeInfoStatic() { static const FlagGG::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \

