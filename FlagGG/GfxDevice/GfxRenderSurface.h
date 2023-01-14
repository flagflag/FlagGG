//
// 抽象图形层RenderSurface
//

#pragma once

#include "GfxDevice/GfxObject.h"

namespace FlagGG
{

class GfxRenderSurface : public GfxObject
{
	OBJECT_OVERRIDE(GfxRenderSurface, GfxObject);
public:
	explicit GfxRenderSurface();

	~GfxRenderSurface() override;
};

}
