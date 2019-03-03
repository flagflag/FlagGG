
#include "Container/VectorBase.h"

namespace FlagGG
{
	namespace Container
	{
		unsigned char* VectorBase::AllocateBuffer(unsigned size)
		{
			return new unsigned char[size];
		}
	}
}
