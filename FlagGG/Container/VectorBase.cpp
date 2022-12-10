
#include "Container/VectorBase.h"

namespace FlagGG
{

unsigned char* VectorBase::AllocateBuffer(unsigned size)
{
	return new unsigned char[size];
}

}
