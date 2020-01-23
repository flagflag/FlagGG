#include "Graphics/ConstantBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		UInt32 ConstantBuffer::GetBindFlags()
		{
			return D3D11_BIND_CONSTANT_BUFFER;
		}
	}
}
