#ifndef __VECTOR3__
#define __VECTOR3__

namespace FlagGG
{
	namespace Math
	{
		struct IntVector3
		{
			IntVector3();

			IntVector3(int x, int y, int z);

			IntVector3(const IntVector3& other);

			int x_;

			int y_;

			int z_;
		};

		struct Vector3
		{
			Vector3();

			Vector3(float x, float y, float z);

			Vector3(const Vector3& other);

			float x_;

			float y_;

			float z_;
		};
	}
}

#endif