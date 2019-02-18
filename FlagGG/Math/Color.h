#ifndef __COLOR__
#define __COLOR__

namespace FlagGG
{
	namespace Math
	{
		struct Color
		{
			Color();

			Color(float r, float g, float b, float a);

			Color(unsigned color);

			unsigned ToUInt() const;

			float r_;

			float g_;

			float b_;

			float a_;
		};
	}
}

#endif
