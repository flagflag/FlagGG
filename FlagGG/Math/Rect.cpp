#include "Math/Rect.h"

#include <cstdio>

namespace FlagGG
{
	namespace Math
	{

		const Rect Rect::FULL(-1.0f, -1.0f, 1.0f, 1.0f);
		const Rect Rect::POSITIVE(0.0f, 0.0f, 1.0f, 1.0f);
		const Rect Rect::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

		const IntRect IntRect::ZERO(0, 0, 0, 0);

		void IntRect::Clip(const IntRect& rect)
		{
			if (rect.left_ > left_)
				left_ = rect.left_;
			if (rect.right_ < right_)
				right_ = rect.right_;
			if (rect.top_ > top_)
				top_ = rect.top_;
			if (rect.bottom_ < bottom_)
				bottom_ = rect.bottom_;

			if (left_ >= right_ || top_ >= bottom_)
				*this = IntRect();
		}

		void IntRect::Merge(const IntRect& rect)
		{
			if (Width() <= 0 || Height() <= 0)
			{
				*this = rect;
			}
			else if (rect.Width() > 0 && rect.Height() > 0)
			{
				if (rect.left_ < left_)
					left_ = rect.left_;
				if (rect.top_ < top_)
					top_ = rect.top_;
				if (rect.right_ > right_)
					right_ = rect.right_;
				if (rect.bottom_ > bottom_)
					bottom_ = rect.bottom_;
			}
		}

		Container::String Rect::ToString() const
		{
			char tempBuffer[Container::CONVERSION_BUFFER_LENGTH];
			sprintf(tempBuffer, "%g %g %g %g", min_.x_, min_.y_, max_.x_, max_.y_);
			return Container::String(tempBuffer);
		}

		Container::String IntRect::ToString() const
		{
			char tempBuffer[Container::CONVERSION_BUFFER_LENGTH];
			sprintf(tempBuffer, "%d %d %d %d", left_, top_, right_, bottom_);
			return Container::String(tempBuffer);
		}

		void Rect::Clip(const Rect& rect)
		{
			if (rect.min_.x_ > min_.x_)
				min_.x_ = rect.min_.x_;
			if (rect.max_.x_ < max_.x_)
				max_.x_ = rect.max_.x_;
			if (rect.min_.y_ > min_.y_)
				min_.y_ = rect.min_.y_;
			if (rect.max_.y_ < max_.y_)
				max_.y_ = rect.max_.y_;

			if (min_.x_ > max_.x_ || min_.y_ > max_.y_)
			{
				min_ = Vector2(F_INFINITY, F_INFINITY);
				max_ = Vector2(-F_INFINITY, -F_INFINITY);
			}
		}

	}
}
