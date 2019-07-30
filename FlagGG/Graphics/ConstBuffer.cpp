#include "Graphics/ConstBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		uint32_t ConstBuffer::GetBindFlags()
		{
			return D3D11_BIND_CONSTANT_BUFFER;
		}
	}
}
