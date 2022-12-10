#include "Graphics/ConstantBuffer.h"

namespace FlagGG
{

UInt32 ConstantBuffer::GetBindFlags()
{
	return D3D11_BIND_CONSTANT_BUFFER;
}

}
