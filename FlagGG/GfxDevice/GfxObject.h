#pragma once

#include "Core/RTTI.h"
#include "Container/RefCounted.h"

namespace FlagGG
{

class GfxObject : public RTTIObject, public RefCounted
{
	OBJECT_OVERRIDE(GfxObject, RTTIObject);
public:
	explicit GfxObject();

	~GfxObject() override;

	virtual bool IsValid() const;
};

}