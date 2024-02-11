#include "Scene/Light.h"
#include "Scene/Node.h"

namespace FlagGG
{

Light::Light()
	: lightType_(LIGHT_TYPE_DIRECTIONAL)
	, lightUnits_(LU_LUMENS)
	, brightness_(3.f)
	, range_(300.f)
	, temperature_(6500.0f)
	, useTemperature_(true)
{ 
	SetNearClip(1.0f);
	SetFarClip(1e5f);
}

Color Light::GetColorFromTemperature() const
{
	// Approximate Planckian locus in CIE 1960 UCS
	float u = (0.860117757f + 1.54118254e-4f * temperature_ + 1.28641212e-7f * temperature_ * temperature_) /
		(1.0f + 8.42420235e-4f * temperature_ + 7.08145163e-7f * temperature_ * temperature_);
	float v = (0.317398726f + 4.22806245e-5f * temperature_ + 4.20481691e-8f * temperature_ * temperature_) /
		(1.0f - 2.89741816e-5f * temperature_ + 1.61456053e-7f * temperature_ * temperature_);

	float x = 3.0f * u / (2.0f * u - 8.0f * v + 4.0f);
	float y = 2.0f * v / (2.0f * u - 8.0f * v + 4.0f);
	float z = 1.0f - x - y;

	float y_ = 1.0f;
	float x_ = y_ / y * x;
	float z_ = y_ / y * z;

	float red = 3.2404542f * x_ + -1.5371385f * y_ + -0.4985314f * z_;
	float green = -0.9692660f * x_ + 1.8760108f * y_ + 0.0415560f * z_;
	float blue = 0.0556434f * x_ + -0.2040259f * y_ + 1.0572252f * z_;

	return Color(red, green, blue);
}

Color Light::GetEffectiveColor() const
{
	float lightBrightness = brightness_;
	if (lightType_ == LIGHT_TYPE_POINT || lightType_ == LIGHT_TYPE_SPOT)
	{
		switch (lightUnits_)
		{
		case LU_CANDELAS:
			lightBrightness *= (100.f * 100.f); // Conversion from cm2 to m2
			break;

		case LU_LUMENS:
			lightBrightness *= (100.f * 100.f / 4 / PI); // Conversion from cm2 to m2 and 4PI from the sphere area in the 1/r2 attenuation
			break;

		case LU_UNITLESS:
			lightBrightness *= 16; // Legacy scale of 16
			break;
		}
	}

	Color effectiveColor(color_ * lightBrightness, 1.0f);
	// 使用色温调制光照颜色
	if (useTemperature_)
	{
		Color tempColor = GetColorFromTemperature();
		effectiveColor.r_ *= tempColor.r_;
		effectiveColor.g_ *= tempColor.g_;
		effectiveColor.b_ *= tempColor.b_;
	}
	return effectiveColor;
}

}
