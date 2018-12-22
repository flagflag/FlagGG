#include "Vector2.h"

namespace FlagGG
{
	namespace Math
	{
		IntVector2::IntVector2() :
			x_(0),
			y_(0)
		{
		}

		IntVector2::IntVector2(int x, int y) :
			x_(x),
			y_(y)
		{

		}

		IntVector2::IntVector2(const IntVector2& other)
		{
			x_ = other.x_;
			y_ = other.y_;
		}

		Vector2::Vector2() :
			x_(0.0f),
			y_(0.0f)
		{
		}

		Vector2::Vector2(float x, float y) :
			x_(x),
			y_(y)
		{
		}

		Vector2::Vector2(const Vector2& other)
		{
			x_ = other.x_;
			y_ = other.y_;
		}
	}
}
