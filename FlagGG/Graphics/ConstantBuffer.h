#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{

class FlagGG_API ConstantBuffer : public GPUBuffer
{
public:
	UInt32 GetBindFlags() override;
};

}
