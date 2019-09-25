#include "Graphics/ConstantBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		uint32_t ConstantBuffer::GetBindFlags()
		{
			return D3D11_BIND_CONSTANT_BUFFER;
		}
	}
}
