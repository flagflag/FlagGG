#ifndef __VECTOR2__
#define __VECTOR2__

#include "Export.h"

namespace FlagGG
{
	namespace Math
	{
		struct FlagGG_API IntVector2
		{
			IntVector2();

			IntVector2(int x, int y);

			IntVector2(const IntVector2& other);

			int x_;

			int y_;
		};

		struct FlagGG_API Vector2
		{
			Vector2();

			Vector2(float x, float y);

			Vector2(const Vector2& other);

			float x_;

			float y_;
		};
	}
}

#endif