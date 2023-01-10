#pragma once

#include "GfxDevice/GfxObject.h"
#include "Graphics/GraphicsDef.h"
#include "Container/FlagSet.h"
#include "Math/Math.h"

namespace FlagGG
{

FLAGGG_FLAGSET(BufferBind, BufferBindFlags);
FLAGGG_FLAGSET(BufferAccess, BufferAccessFlags);

// Total 8 bit
struct GfxBufferDesc
{
	UInt32 size_;
	UInt16 stride_;
	UInt8 bindFlags_;      // BufferBind
	UInt8 accessFlags_;    // BufferAccess
	UInt8 usage_;          // BufferUsage
};

class GfxBuffer : public GfxObject
{
	OBJECT_OVERRIDE(GfxBuffer, GfxObject);
public:
	explicit GfxBuffer();

	~GfxBuffer() override;

	// 设置Stride
	virtual void SetStride(UInt16 stride);

	// 设置buffer大小
	virtual void SetSize(UInt32 size);

	// 设置buffer绑定类型
	virtual void SetBind(BufferBindFlags bindFlags);

	// 设置buffer权限
	virtual void SetAccess(BufferAccessFlags bindFlags);

	// 设置buffer用法
	virtual void SetUsage(BufferUsage usage);

	// 应用当前设置（未调用之前buffer处于不可用状态）
	virtual void Apply(const void* initialDataPtr);

	// 更新buffer的数据（usage == BUFFER_DYANMIC时可用）
	virtual void UpdateBuffer(const void* dataPtr);

	// 更新buffer一个范围内的数据（usage == BUFFER_DYANMIC时可用）
	virtual void UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size);

	// 开始写数据
	virtual void* BeginWrite(UInt32 offset, UInt32 size);

	// 结束写数据
	virtual void EndWrite(UInt32 bytesWritten);

	// 获取buffer描述
	const GfxBufferDesc& GetDesc() const { return gfxBufferDesc_; }

protected:
	// buffer的描述
	GfxBufferDesc gfxBufferDesc_;
};

}
