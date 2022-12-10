#include "Color.h"
#include "Math/Math.h"

#include <math.h>
#include <assert.h>

namespace FlagGG
{

unsigned Color::ToUInt() const
{
	auto r = (unsigned)Clamp(((int)(r_ * 255.0f)), 0, 255);
	auto g = (unsigned)Clamp(((int)(g_ * 255.0f)), 0, 255);
	auto b = (unsigned)Clamp(((int)(b_ * 255.0f)), 0, 255);
	auto a = (unsigned)Clamp(((int)(a_ * 255.0f)), 0, 255);
	return (a << 24u) | (b << 16u) | (g << 8u) | r;
}

Vector3 Color::ToHSL() const
{
	Real min, max;
	Bounds(&min, &max, true);

	Real h = Hue(min, max);
	Real s = SaturationHSL(min, max);
	Real l = (max + min) * 0.5f;

	return Vector3(h, s, l);
}

Vector3 Color::ToHSV() const
{
	Real min, max;
	Bounds(&min, &max, true);

	Real h = Hue(min, max);
	Real s = SaturationHSV(min, max);
	Real v = max;

	return Vector3(h, s, v);
}

void Color::FromUInt(unsigned color)
{
	a_ = ((color >> 24u) & 0xffu) / 255.0f;
	b_ = ((color >> 16u) & 0xffu) / 255.0f;
	g_ = ((color >> 8u) & 0xffu) / 255.0f;
	r_ = ((color >> 0u) & 0xffu) / 255.0f;
}

void Color::FromHSL(Real h, Real s, Real l, Real a)
{
	Real c;
	if (l < 0.5f)
		c = (1.0f + (2.0f * l - 1.0f)) * s;
	else
		c = (1.0f - (2.0f * l - 1.0f)) * s;

	Real m = l - 0.5f * c;

	FromHCM(h, c, m);

	a_ = a;
}

void Color::FromHSV(Real h, Real s, Real v, Real a)
{
	Real c = v * s;
	Real m = v - c;

	FromHCM(h, c, m);

	a_ = a;
}

Real Color::Chroma() const
{
	Real min, max;
	Bounds(&min, &max, true);

	return max - min;
}

Real Color::Hue() const
{
	Real min, max;
	Bounds(&min, &max, true);

	return Hue(min, max);
}

Real Color::SaturationHSL() const
{
	Real min, max;
	Bounds(&min, &max, true);

	return SaturationHSL(min, max);
}

Real Color::SaturationHSV() const
{
	Real min, max;
	Bounds(&min, &max, true);

	return SaturationHSV(min, max);
}

Real Color::Lightness() const
{
	Real min, max;
	Bounds(&min, &max, true);

	return (max + min) * 0.5f;
}

void Color::Bounds(Real* min, Real* max, bool clipped) const
{
	assert(min && max);

	if (r_ > g_)
	{
		if (g_ > b_) // r > g > b
		{
			*max = r_;
			*min = b_;
		}
		else // r > g && g <= b
		{
			*max = r_ > b_ ? r_ : b_;
			*min = g_;
		}
	}
	else
	{
		if (b_ > g_) // r <= g < b
		{
			*max = b_;
			*min = r_;
		}
		else // r <= g && b <= g
		{
			*max = g_;
			*min = r_ < b_ ? r_ : b_;
		}
	}

	if (clipped)
	{
		*max = *max > 1.0f ? 1.0f : (*max < 0.0f ? 0.0f : *max);
		*min = *min > 1.0f ? 1.0f : (*min < 0.0f ? 0.0f : *min);
	}
}

Real Color::MaxRGB() const
{
	if (r_ > g_)
		return (r_ > b_) ? r_ : b_;
	else
		return (g_ > b_) ? g_ : b_;
}

Real Color::MinRGB() const
{
	if (r_ < g_)
		return (r_ < b_) ? r_ : b_;
	else
		return (g_ < b_) ? g_ : b_;
}

Real Color::Range() const
{
	Real min, max;
	Bounds(&min, &max);
	return max - min;
}

void Color::Clip(bool clipAlpha)
{
	r_ = (r_ > 1.0f) ? 1.0f : ((r_ < 0.0f) ? 0.0f : r_);
	g_ = (g_ > 1.0f) ? 1.0f : ((g_ < 0.0f) ? 0.0f : g_);
	b_ = (b_ > 1.0f) ? 1.0f : ((b_ < 0.0f) ? 0.0f : b_);

	if (clipAlpha)
		a_ = (a_ > 1.0f) ? 1.0f : ((a_ < 0.0f) ? 0.0f : a_);
}

void Color::Invert(bool invertAlpha)
{
	r_ = 1.0f - r_;
	g_ = 1.0f - g_;
	b_ = 1.0f - b_;

	if (invertAlpha)
		a_ = 1.0f - a_;
}

Color Color::Lerp(const Color& rhs, Real t) const
{
	Real invT = 1.0f - t;
	return Color(
		r_ * invT + rhs.r_ * t,
		g_ * invT + rhs.g_ * t,
		b_ * invT + rhs.b_ * t,
		a_ * invT + rhs.a_ * t
		);
}

String Color::ToString() const
{
	char tempBuffer[CONVERSION_BUFFER_LENGTH];
	sprintf(tempBuffer, "%g %g %g %g", r_, g_, b_, a_);
	return String(tempBuffer);
}

Real Color::Hue(Real min, Real max) const
{
	Real chroma = max - min;

	// If chroma equals zero, hue is undefined
	if (chroma <= EPS)
		return 0.0f;

	// Calculate and return hue
	if (FlagGG::Equals(g_, max))
		return (b_ + 2.0f * chroma - r_) / (6.0f * chroma);
	else if (FlagGG::Equals(b_, max))
		return (4.0f * chroma - g_ + r_) / (6.0f * chroma);
	else
	{
		Real r = (g_ - b_) / (6.0f * chroma);
		return (r < 0.0f) ? 1.0f + r : ((r >= 1.0f) ? r - 1.0f : r);
	}

}

Real Color::SaturationHSV(Real min, Real max) const
{
	// 避免除零，结果不正确
	if (max <= EPS)
		return 0.0f;

	return 1.0f - (min / max);
}

Real Color::SaturationHSL(Real min, Real max) const
{
	// 避免除零，结果不正确
	if (max <= EPS || min >= 1.0f - EPS)
		return 0.0f;

	Real hl = (max + min);
	if (hl <= 1.0f)
		return (max - min) / hl;
	else
		return (min - max) / (hl - 2.0f);

}

void Color::FromHCM(Real h, Real c, Real m)
{
	if (h < 0.0f || h >= 1.0f)
		h -= floorf(h);

	Real hs = h * 6.0f;
	Real x = c * (1.0f - FlagGG::Abs(fmodf(hs, 2.0f) - 1.0f));

	if (hs < 2.0f)
	{
		b_ = 0.0f;
		if (hs < 1.0f)
		{
			g_ = x;
			r_ = c;
		}
		else
		{
			g_ = c;
			r_ = x;
		}
	}
	else if (hs < 4.0f)
	{
		r_ = 0.0f;
		if (hs < 3.0f)
		{
			g_ = c;
			b_ = x;
		}
		else
		{
			g_ = x;
			b_ = c;
		}
	}
	else
	{
		g_ = 0.0f;
		if (hs < 5.0f)
		{
			r_ = x;
			b_ = c;
		}
		else
		{
			r_ = c;
			b_ = x;
		}
	}

	r_ += m;
	g_ += m;
	b_ += m;
}

const Color Color::WHITE;
const Color Color::GRAY(0.5f, 0.5f, 0.5f);
const Color Color::BLACK(0.0f, 0.0f, 0.0f);
const Color Color::RED(1.0f, 0.0f, 0.0f);
const Color Color::GREEN(0.0f, 1.0f, 0.0f);
const Color Color::BLUE(0.0f, 0.0f, 1.0f);
const Color Color::CYAN(0.0f, 1.0f, 1.0f);
const Color Color::MAGENTA(1.0f, 0.0f, 1.0f);
const Color Color::YELLOW(1.0f, 1.0f, 0.0f);
const Color Color::TRANSPARENT_BLACK(0.0f, 0.0f, 0.0f, 0.0f);

}
