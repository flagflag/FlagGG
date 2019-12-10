#ifndef __COLOR__
#define __COLOR__

#include "Export.h"
#include "Define.h"
#include "Container/Str.h"
#include "Math/Math.h"
#include "Math/Vector3.h"

namespace FlagGG
{
	namespace Math
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

			Color(const Color& color, float a) NOEXCEPT :
			r_(color.r_),
				g_(color.g_),
				b_(color.b_),
				a_(a)
			{
			}

			Color(float r, float g, float b) NOEXCEPT :
			r_(r),
				g_(g),
				b_(b),
				a_(1.0f)
			{
			}

			Color(float r, float g, float b, float a) NOEXCEPT :
			r_(r),
				g_(g),
				b_(b),
				a_(a)
			{
			}

			explicit Color(const float* data) NOEXCEPT :
			r_(data[0]),
				g_(data[1]),
				b_(data[2]),
				a_(data[3])
			{
			}

			Color& operator =(const Color& rhs) NOEXCEPT = default;

			bool operator ==(const Color& rhs) const { return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_; }

			bool operator !=(const Color& rhs) const { return r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_; }

			Color operator *(float rhs) const { return Color(r_ * rhs, g_ * rhs, b_ * rhs, a_ * rhs); }

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

			const float* Data() const { return &r_; }

			// 把颜色压缩成32位整数, Red在低八位.
			unsigned ToUInt() const;
			Vector3 ToHSL() const;
			Vector3 ToHSV() const;

			// ToUInt的逆
			void FromUInt(unsigned color);
			void FromHSL(float h, float s, float l, float a = 1.0f);
			void FromHSV(float h, float s, float v, float a = 1.0f);

			Vector3 ToVector3() const { return Vector3(r_, g_, b_); }

			float SumRGB() const { return r_ + g_ + b_; }

			float Average() const { return (r_ + g_ + b_) / 3.0f; }

			float Luma() const { return r_ * 0.299f + g_ * 0.587f + b_ * 0.114f; }

			float Chroma() const;
			float Hue() const;
			float SaturationHSL() const;
			float SaturationHSV() const;

			float Value() const { return MaxRGB(); }

			float Lightness() const;

			void Bounds(float* min, float* max, bool clipped = false) const;
			float MaxRGB() const;
			float MinRGB() const;
			float Range() const;

			void Clip(bool clipAlpha = false);
			void Invert(bool invertAlpha = false);
			Color Lerp(const Color& rhs, float t) const;

			Color Abs() const { return Color(Math::Abs(r_), Math::Abs(g_), Math::Abs(b_), Math::Abs(a_)); }

			bool Equals(const Color& rhs) const
			{
				return Math::Equals(r_, rhs.r_) && Math::Equals(g_, rhs.g_) && Math::Equals(b_, rhs.b_) && Math::Equals(a_, rhs.a_);
			}

			Container::String ToString() const;

			unsigned ToHash() const { return ToUInt(); }

			float r_;
			float g_;
			float b_;
			float a_;

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
			float Hue(float min, float max) const;
			float SaturationHSV(float min, float max) const;
			float SaturationHSL(float min, float max) const;
			void FromHCM(float h, float c, float m);
		};

		inline Color operator *(float lhs, const Color& rhs) { return rhs * lhs; }
	}
}

#endif
