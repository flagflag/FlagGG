#pragma once

#include "Core/Object.h"
#include "Resource/StreamObject.h"

namespace FlagGG
{

class FlagGG_API Resource : public Object, public StreamObject
{
	OBJECT_OVERRIDE(Resource, Object);
public:
	Resource();

	~Resource() override = default;

	void SetName(const String& name);

	const String& GetName() const { return name_; }

	void SetMemoryUse(UInt32 memoryUse);

	UInt32 GetMemoryUse() const;

protected:
	String name_;

	UInt32 memoryUse_{ 0 };
};

}
