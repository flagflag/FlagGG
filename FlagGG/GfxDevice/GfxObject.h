//
// 抽象图形层对象
//

#pragma once

#include "Core/RTTI.h"
#include "Container/RefCounted.h"

namespace FlagGG
{

class FlagGG_API GfxObject : public RTTIObject, public RefCounted
{
	OBJECT_OVERRIDE(GfxObject, RTTIObject);
public:
	explicit GfxObject();

	~GfxObject() override;

	virtual bool IsValid() const;
};

}
