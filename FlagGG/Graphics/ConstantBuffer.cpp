#include "Graphics/ConstantBuffer.h"

namespace FlagGG
{

void ConstantBuffer::SetSize(UInt32 size)
{
	gfxBuffer_->SetStride(4u);
	gfxBuffer_->SetSize(size);
	gfxBuffer_->SetBind(BUFFER_UNIFORM);
	gfxBuffer_->SetAccess(BUFFER_WRITE);
	gfxBuffer_->SetUsage(BUFFER_STATIC);
	gfxBuffer_->Apply(nullptr);
}

}
