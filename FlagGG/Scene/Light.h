//
// 灯光控件
//

#pragma once

#include "Export.h"
#include "Scene/Camera.h"

namespace FlagGG
{

enum LightType
{
	LIGHT_TYPE_DIRECTIONAL = 0,
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT,
	LIGHT_TYPE_AREA,
};

class FlagGG_API Light : public Camera
{
	OBJECT_OVERRIDE(Light, Camera);
public:
	Light();

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_LIGHT; }

	// 获取灯光类型
	LightType GetLightType() const { return lightType_; }

private:
	LightType lightType_;
};

}
