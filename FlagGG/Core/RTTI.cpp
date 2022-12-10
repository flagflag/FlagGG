#include "RTTI.h"

namespace FlagGG
{

TypeInfo::TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo) :
	type_(typeName),
	typeName_(typeName),
	baseTypeInfo_(baseTypeInfo)
{
}

TypeInfo::~TypeInfo() = default;

bool TypeInfo::IsTypeOf(StringHash type) const
{
	const TypeInfo* current = this;
	while (current)
	{
		if (current->GetType() == type)
			return true;

		current = current->GetBaseTypeInfo();
	}

	return false;
}

bool TypeInfo::IsTypeOf(const TypeInfo* typeInfo) const
{
	const TypeInfo* current = this;
	while (current)
	{
		if (current == typeInfo)
			return true;

		current = current->GetBaseTypeInfo();
	}

	return false;
}

RTTIObject::~RTTIObject() = default;

bool RTTIObject::IsInstanceOf(StringHash type) const
{
	return GetTypeInfo()->IsTypeOf(type);
}

bool RTTIObject::IsInstanceOf(const TypeInfo* typeInfo) const
{
	return GetTypeInfo()->IsTypeOf(typeInfo);
}

}
