#include "GfxBufferOGL.h"

namespace FlagGG
{

GfxBufferOpenGL::GfxBufferOpenGL()
	: GfxBuffer()
{

}

GfxBufferOpenGL::~GfxBufferOpenGL()
{
	ReleaseBuffer();
}

void GfxBufferOpenGL::Apply(const void* initialDataPtr)
{
	ReleaseBuffer();

	oglTarget_ = 0;

	if ((gfxBufferDesc_.bindFlags_ & BUFFER_BIND_VERTEX) ||
		(gfxBufferDesc_.bindFlags_ & BUFFER_BIND_INDEX))
		oglTarget_ = GL_ARRAY_BUFFER;
	else if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_UNIFORM)
		oglTarget_ = GL_UNIFORM_BUFFER;

	oglUsage_ = (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	GL::GenBuffers(1, &oglBuffer_);
	GL::BindBuffer(oglTarget_, oglBuffer_);
	GL::BufferData(oglTarget_, gfxBufferDesc_.size_, initialDataPtr, oglUsage_);
	GL::BindBuffer(oglTarget_, 0);
}

void GfxBufferOpenGL::UpdateBuffer(const void* dataPtr)
{
	// 静态buffer不修改数据
	if (gfxBufferDesc_.usage_ == BUFFER_USAGE_STATIC)
	{
		CRY_ASSERT(false);
		return;
	}

	GL::BindBuffer(oglTarget_, oglBuffer_);
	GL::BufferData(oglTarget_, gfxBufferDesc_.size_, dataPtr, oglUsage_);
	GL::BindBuffer(oglTarget_, 0);
}

void GfxBufferOpenGL::UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size)
{
	// 静态buffer不修改数据
	if (gfxBufferDesc_.usage_ == BUFFER_USAGE_STATIC)
	{
		CRY_ASSERT(false);
		return;
	}

	GL::BindBuffer(oglTarget_, oglBuffer_);
	GL::BufferSubData(oglTarget_, offset, size, dataPtr);
	GL::BindBuffer(oglTarget_, 0);
}

void* GfxBufferOpenGL::BeginWrite(UInt32 offset, UInt32 size)
{
	if (!oglBuffer_)
	{
		CRY_ASSERT(false);
		return nullptr;
	}

	if (offset + size > gfxBufferDesc_.size_)
	{
		CRY_ASSERT(false);
		return nullptr;
	}

	writeOffset_ = offset;

	shadowdData_.Resize(size);
	return &shadowdData_[0];
}

void GfxBufferOpenGL::EndWrite(UInt32 bytesWritten)
{
	if (!oglBuffer_)
	{
		CRY_ASSERT(false);
		return;
	}

	GL::BindBuffer(oglTarget_, oglBuffer_);
// 取决于是否要用discard buffer的写法
#if 0
	if (writeOffset_ == 0)
		GL::BufferData(oglTarget_, shadowdData_.Size(), shadowdData_.Buffer(), oglUsage_);
	else
#endif
		GL::BufferSubData(oglTarget_, writeOffset_, Min(bytesWritten, shadowdData_.Size()), shadowdData_.Buffer());
	GL::BindBuffer(oglTarget_, 0);
}

void GfxBufferOpenGL::ReleaseBuffer()
{
	if (oglBuffer_)
	{
		GL::BindBuffer(oglTarget_, 0);
		GL::DeleteBuffers(1, &oglBuffer_);
	}
}

}
