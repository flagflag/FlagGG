#include "Math.h"

#include <math.h>

namespace FlagGG
{
	namespace Math
	{
		static const float EPS = 1e-5;
		static const float PI = 3.1415926;

		bool IsNaN(float number)
		{
			return isnan(number);
		}

		float Sin(float angle)
		{
			return sin(angle * 180.0f / PI);
		}

		float Cos(float angle)
		{
			return cos(angle * 180.0f / PI);
		}

		float Tan(float angle)
		{
			return tan(angle * 180.0f / PI);
		}

		float Asin(float angle)
		{
			return asin(angle * 180.0f / PI);
		}

		float Acos(float angle)
		{
			return acos(angle * 180.0f / PI);
		}

		float Atan(float angle)
		{
			return atan(angle * 180.0f / PI);
		}

		float Abs(float number)
		{
			return fabs(number);
		}

		float Sqrt(float number)
		{
			return sqrt(number);
		}

		int Compare(float _1, float _2)
		{
			if (fabs(_1 - _2) <  EPS) return 0;
			return _1 < _2 ? -1 : 1;
		}

		float Equals(float _1, float _2)
		{
			return Compare(_1, _2) == 0;
		}

		float Clamp(float target, float min, float max)
		{
			if (target < min)
				return min;
			else if (target > max)
				return max;
			else
				return target;
		}
	}
}
