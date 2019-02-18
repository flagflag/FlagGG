#include "Color.h"
#include "Math/Math.h"

namespace FlagGG
{
	namespace Math
	{
		Color::Color() :
			r_(1.0f),
			g_(1.0f),
			b_(1.0f),
			a_(1.0f)
		{ }

		Color::Color(float r, float g, float b, float a = 1.0f) :
			r_(r),
			g_(g),
			b_(b),
			a_(a)
		{ }

		Color::Color(unsigned color) :
			r_(((color >> 0u)  & 0xffu) / 255.0f),
			g_(((color >> 8u)  & 0xffu) / 255.0f),
			b_(((color >> 16u) & 0xffu) / 255.0f),
			a_(((color >> 24u) & 0xffu) / 255.0f)
		{ }

		unsigned Color::ToUInt() const
		{
			unsigned r = (unsigned)Clamp((int)(r_ * 255.0f), 0, 255);
			unsigned g = (unsigned)Clamp((int)(g_ * 255.0f), 0, 255);
			unsigned b = (unsigned)Clamp((int)(b_ * 255.0f), 0, 255);
			unsigned a = (unsigned)Clamp((int)(a_ * 255.0f), 0, 255);
			return (r << 0u) | (g << 8u) | (b << 16u) | (a << 24u);
		}
	}
}
