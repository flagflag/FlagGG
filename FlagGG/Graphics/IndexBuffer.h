#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{

class FlagGG_API IndexBuffer : public GPUBuffer
{
public:
	UInt32 GetBindFlags() override;

	bool SetSize(UInt32 indexSize, UInt32 indexCount);

	UInt32 GetIndexSize() const;

	UInt32 GetIndexCount() const;

protected:

	UInt32 indexSize_{ 0 };
	UInt32 indexCount_{ 0 };
};

}

