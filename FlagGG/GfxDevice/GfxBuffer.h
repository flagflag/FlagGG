//
// 抽象图形层Buffer
//

#pragma once

#include "GfxDevice/GfxObject.h"
#include "Graphics/GraphicsDef.h"
#include "Container/Ptr.h"
#include "Container/FlagSet.h"
#include "Math/Math.h"

namespace FlagGG
{

FLAGGG_FLAGSET(BufferBind, BufferBindFlags);
FLAGGG_FLAGSET(BufferAccess, BufferAccessFlags);

struct GfxBufferDesc
{
	UInt32 size_;
	UInt16 stride_;
	BufferBindFlags bindFlags_;      // BufferBind
	BufferAccessFlags accessFlags_;  // BufferAccess
	BufferUsage usage_;              // BufferUsage
};

class FlagGG_API GfxBufferReadbackDataStream : public GfxObject
{
	OBJECT_OVERRIDE(GfxBufferReadbackDataStream, GfxObject);
public:
	explicit GfxBufferReadbackDataStream();

	~GfxBufferReadbackDataStream() override;

	virtual void Read(void* dataPtr) = 0;
};

class FlagGG_API GfxBuffer : public GfxObject
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

	// 拷贝数据
	virtual void CopyData(GfxBuffer* srcBuffer, UInt32 srcOffset, UInt32 destOffset, UInt32 copySize);

	// 回读GPU数据
	virtual bool ReadBack(void* dataPtr);

	// 回读GPU数据（某个Rect区域）
	virtual bool ReadBackSubResigon(void* dataPtr, UInt32 offset, UInt32 size);

	// 回读GPU数据
	virtual SharedPtr<GfxBufferReadbackDataStream> ReadBackToStream();

	// 回去GPU数据（某个Rect区域）
	virtual SharedPtr<GfxBufferReadbackDataStream> ReadbackToStream(UInt32 offset, UInt32 size);

	// 获取CPU映射数据
	virtual const UInt8* GetShadowData() const;

	// 获取buffer描述
	const GfxBufferDesc& GetDesc() const { return gfxBufferDesc_; }

protected:
	// buffer的描述
	GfxBufferDesc gfxBufferDesc_;
};

}
