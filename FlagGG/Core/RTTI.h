#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/StringHash.h"

namespace FlagGG
{
	namespace Core
	{
		class FlagGG_API TypeInfo
		{
		public:
			TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo);
			~TypeInfo();

			bool IsTypeOf(Container::StringHash type) const;
			bool IsTypeOf(const TypeInfo* typeInfo) const;
			template<typename T> bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

			Container::StringHash GetType() const { return type_; }
			const Container::String& GetTypeName() const { return typeName_; }
			const TypeInfo* GetBaseTypeInfo() const { return baseTypeInfo_; }

		private:
			Container::StringHash type_;
			Container::String typeName_;
			const TypeInfo* baseTypeInfo_;
		};

		class FlagGG_API RTTIObject
		{
		public:
			virtual ~RTTIObject();

			virtual Container::StringHash GetType() const = 0;
			virtual const Container::String& GetTypeName() const = 0;
			virtual const TypeInfo* GetTypeInfo() const = 0;

			static const TypeInfo* GetTypeInfoStatic() { return nullptr; }
			bool IsInstanceOf(Container::StringHash type) const;
			bool IsInstanceOf(const TypeInfo* typeInfo) const;
			template<typename T> bool IsInstanceOf() const { return IsInstanceOf(T::GetTypeInfoStatic()); }
			template<typename T> T* DynamicCast() { return IsInstanceOf<T>() ? static_cast<T*>(this) : nullptr; }
			template<typename T> const T* DynamicCast() const { return IsInstanceOf<T>() ? static_cast<const T*>(this) : nullptr; }
		};
	}
}

#define OBJECT_OVERRIDE(typeName, baseTypeName) \
	public: \
        using ClassName = typeName; \
        using BaseClassName = baseTypeName; \
		using SuperClassName = baseTypeName; \
        virtual FlagGG::Container::StringHash GetType() const override { return GetTypeInfoStatic()->GetType(); } \
        virtual const FlagGG::Container::String& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
        virtual const FlagGG::Core::TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
        static FlagGG::Container::StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
        static const FlagGG::Container::String& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
        static const FlagGG::Core::TypeInfo* GetTypeInfoStatic() { static const FlagGG::Core::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \

