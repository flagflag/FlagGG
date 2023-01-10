#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{

class FlagGG_API ConstantBuffer : public GPUBuffer
{
public:
	void SetSize(UInt32 size);
};

}
