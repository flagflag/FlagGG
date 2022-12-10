#ifndef __COLOR__
#define __COLOR__

#include "Export.h"
#include "Define.h"
#include "Container/Str.h"
#include "Math/Math.h"
#include "Math/Vector3.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API Color
{
public:
	Color() NOEXCEPT :
		r_(1.0f),
		g_(1.0f),
		b_(1.0f),
		a_(1.0f)
	{
	}

	Color(const Color& color) NOEXCEPT = default;

	Color(const Color& color, Real a) NOEXCEPT :
	r_(color.r_),
		g_(color.g_),
		b_(color.b_),
		a_(a)
	{
	}

	Color(Real r, Real g, Real b) NOEXCEPT :
	r_(r),
		g_(g),
		b_(b),
		a_(1.0f)
	{
	}

	Color(Real r, Real g, Real b, Real a) NOEXCEPT :
	r_(r),
		g_(g),
		b_(b),
		a_(a)
	{
	}

	explicit Color(const Real* data) NOEXCEPT :
	r_(data[0]),
		g_(data[1]),
		b_(data[2]),
		a_(data[3])
	{
	}

	Color& operator =(const Color& rhs) NOEXCEPT = default;

	bool operator ==(const Color& rhs) const { return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_; }

	bool operator !=(const Color& rhs) const { return r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_; }

	Color operator *(Real rhs) const { return Color(r_ * rhs, g_ * rhs, b_ * rhs, a_ * rhs); }

	Color operator +(const Color& rhs) const { return Color(r_ + rhs.r_, g_ + rhs.g_, b_ + rhs.b_, a_ + rhs.a_); }

	Color operator -() const { return Color(-r_, -g_, -b_, -a_); }

	Color operator -(const Color& rhs) const { return Color(r_ - rhs.r_, g_ - rhs.g_, b_ - rhs.b_, a_ - rhs.a_); }

	Color& operator +=(const Color& rhs)
	{
		r_ += rhs.r_;
		g_ += rhs.g_;
		b_ += rhs.b_;
		a_ += rhs.a_;
		return *this;
	}

	const Real* Data() const { return &r_; }

	// 把颜色压缩成32位整数, Red在低八位.
	unsigned ToUInt() const;
	Vector3 ToHSL() const;
	Vector3 ToHSV() const;

	// ToUInt的逆
	void FromUInt(unsigned color);
	void FromHSL(Real h, Real s, Real l, Real a = 1.0f);
	void FromHSV(Real h, Real s, Real v, Real a = 1.0f);

	Vector3 ToVector3() const { return Vector3(r_, g_, b_); }

	Real SumRGB() const { return r_ + g_ + b_; }

	Real Average() const { return (r_ + g_ + b_) / 3.0f; }

	Real Luma() const { return r_ * 0.299f + g_ * 0.587f + b_ * 0.114f; }

	Real Chroma() const;
	Real Hue() const;
	Real SaturationHSL() const;
	Real SaturationHSV() const;

	Real Value() const { return MaxRGB(); }

	Real Lightness() const;

	void Bounds(Real* min, Real* max, bool clipped = false) const;
	Real MaxRGB() const;
	Real MinRGB() const;
	Real Range() const;

	void Clip(bool clipAlpha = false);
	void Invert(bool invertAlpha = false);
	Color Lerp(const Color& rhs, Real t) const;

	Color Abs() const { return Color(FlagGG::Abs(r_), FlagGG::Abs(g_), FlagGG::Abs(b_), FlagGG::Abs(a_)); }

	bool Equals(const Color& rhs) const
	{
		return FlagGG::Equals(r_, rhs.r_) && FlagGG::Equals(g_, rhs.g_) && FlagGG::Equals(b_, rhs.b_) && FlagGG::Equals(a_, rhs.a_);
	}

	String ToString() const;

	unsigned ToHash() const { return ToUInt(); }

	Real r_;
	Real g_;
	Real b_;
	Real a_;

	static const Color WHITE;
	static const Color GRAY;
	static const Color BLACK;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color CYAN;
	static const Color MAGENTA;
	static const Color YELLOW;
	static const Color TRANSPARENT_BLACK;

protected:
	Real Hue(Real min, Real max) const;
	Real SaturationHSV(Real min, Real max) const;
	Real SaturationHSL(Real min, Real max) const;
	void FromHCM(Real h, Real c, Real m);
};

inline Color operator *(Real lhs, const Color& rhs) { return rhs * lhs; }

}

#endif
