#ifndef __MATH__
#define __MATH__

namespace FlagGG
{
	namespace Math
	{
		bool IsNaN(float number);

		float Sin(float angle);

		float Cos(float angle);

		float Tan(float angle);

		float Asin(float angle);

		float Acos(float angle);

		float Atan(float angle);


		float Abs(float number);

		float Sqrt(float number);


		int Compare(float _1, float _2);

		float Equals(float _1, float _2);

		float Clamp(float target, float min, float max);
	}
}

#endif