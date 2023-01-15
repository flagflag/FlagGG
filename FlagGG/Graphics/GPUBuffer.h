//
// 引擎层Buffer子类
//

#pragma once
#include "Export.h"
#include "Graphics/GraphicsDef.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Core/BaseTypes.h"
#include "GfxDevice/GfxBuffer.h"

namespace FlagGG
{

class FlagGG_API GPUBuffer : public RefCounted
{
public:
	explicit GPUBuffer();

	~GPUBuffer() override;

	void* Lock(UInt32 start, UInt32 count);

	void Unlock();

	IOFrame::Buffer::IOBuffer* LockStaticBuffer(UInt32 start, UInt32 count);

	void UnlockStaticBuffer();

	// 获取gfx引用
	GfxBuffer* GetGfxRef() const { return gfxBuffer_; }

protected:
	SharedPtr<IOFrame::Buffer::IOBuffer> buffer_;

	SharedPtr<GfxBuffer> gfxBuffer_;

	UInt32 lockSize_{};
};

}
