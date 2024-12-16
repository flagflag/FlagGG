#pragma once

#include "Export.h"
#include "Core/RTTI.h"

namespace FlagGG
{

class FlagGG_API Object : public RefCounted, public RTTIObject
{
	OBJECT_OVERRIDE(Object, RTTIObject);
public:
	~Object() override;
};

class FlagGG_API ThreadSafeObject : public ThreadSafeRefCounted, public RTTIObject
{
public:
	~ThreadSafeObject() override;
};

}


