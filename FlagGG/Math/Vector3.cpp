#include "Vector3.h"

namespace FlagGG
{
	namespace Math
	{
		IntVector3::IntVector3() :
			x_(0),
			y_(0),
			z_(0)
		{
		}

		IntVector3::IntVector3(int x, int y, int z) :
			x_(x),
			y_(y),
			z_(z)
		{
		}

		IntVector3::IntVector3(const IntVector3& other)
		{
			x_ = other.x_;
			y_ = other.y_;
			z_ = other.z_;
		}

		Vector3::Vector3() :
			x_(0.0f),
			y_(0.0f),
			z_(0.0f)
		{
		}

		Vector3::Vector3(float x, float y, float z) :
			x_(x),
			y_(y),
			z_(z)
		{
		}

		Vector3::Vector3(const Vector3& other)
		{
			x_ = other.x_;
			y_ = other.y_;
			z_ = other.z_;
		}
	}
}
