#include "Ultralight/Geometry.h"

#include <Math/Math.h>

using namespace FlagGG;

namespace ultralight
{

void RoundedRect::SetEmpty()
{
	rect.SetEmpty();
	radii_x[0] = radii_x[1] = radii_x[2] = radii_x[3] = 0.0f;
	radii_y[0] = radii_y[1] = radii_y[2] = radii_y[3] = 0.0f;
}

bool RoundedRect::IsRounded() const
{
	return
		Equals(radii_x[0], 0.0f) &&
		Equals(radii_x[1], 0.0f) &&
		Equals(radii_x[2], 0.0f) &&
		Equals(radii_x[3], 0.0f) &&
		Equals(radii_y[0], 0.0f) &&
		Equals(radii_y[1], 0.0f) &&
		Equals(radii_y[2], 0.0f) &&
		Equals(radii_y[3], 0.0f);
}

float RoundedRect::GetSignedDistance(const Point& p) const
{
	return 0.0f;
}

bool RoundedRect::Intersect(const RoundedRect& other, RoundedRect& result) const
{
	return false;
}

void RoundedRect::SnapToPixels()
{

}

Rect RoundedRect::CalculateInterior() const
{
	return Rect();
}

}
