//
// 灯光控件
//

#pragma once

#include "Export.h"
#include "Scene/Camera.h"
#include "Math/Color.h"

namespace FlagGG
{

enum LightType
{
	LIGHT_TYPE_DIRECTIONAL = 0,
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT,
	LIGHT_TYPE_AREA,
};

/// Light units.
enum LightUnits
{
	LU_UNITLESS = 0,
	LU_CANDELAS,
	LU_LUMENS,
};

class FlagGG_API Light : public Camera
{
	OBJECT_OVERRIDE(Light, Camera);
public:
	Light();

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_LIGHT; }

	// 设置灯管类型
	void SetLightType(LightType lightType) { lightType_ = lightType; }

	// 获取灯光类型
	LightType GetLightType() const { return lightType_; }

	// 设置光照强度单位
	void SetLightUnit(LightUnits lightUnits) { lightUnits_ = lightUnits; }

	// 获取光照强度单位
	LightUnits GetLightUnits() const { return lightUnits_; }

	// 设置灯光颜色
	void SetColor(const Color& color) { color_ = color; }

	// 获取灯光颜色
	const Color& GetColor() const { return color_; }

	// 设置灯光亮度
	void SetBrightness(float brightness) { brightness_ = brightness; }

	// 获取灯光亮度
	float GetBrightness() const { return brightness_; }

	// 设置灯光范围
	void SetRange(float range) { range_ = range; }

	// 获取灯光范围
	float GetRange() const { return range_; }

	// 设置源半径
	void SetSourceRadius(float sourceRadius) { sourceRadius_ = sourceRadius; }

	// 获取源半径
	float GetSourceRadius() const { return sourceRadius_; }

	// 获取实际的光照强度颜色
	Color GetEffectiveColor() const;

protected:
	Color GetColorFromTemperature() const;

private:
	LightType lightType_;

	LightUnits lightUnits_;

	Color color_;

	float brightness_;

	float range_;

	float sourceRadius_;

	// 色温
	float temperature_;

	// 是否使用色温
	bool useTemperature_;
};

}
