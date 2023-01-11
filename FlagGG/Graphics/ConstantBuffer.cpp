#include "Graphics/ConstantBuffer.h"

namespace FlagGG
{

void ConstantBuffer::SetSize(UInt32 size)
{
	gfxBuffer_->SetStride(4u);
	gfxBuffer_->SetSize(size);
	gfxBuffer_->SetBind(BUFFER_BIND_UNIFORM);
	gfxBuffer_->SetAccess(BUFFER_ACCESS_NONE);
	gfxBuffer_->SetUsage(BUFFER_USAGE_STATIC);
	gfxBuffer_->Apply(nullptr);
}

}
