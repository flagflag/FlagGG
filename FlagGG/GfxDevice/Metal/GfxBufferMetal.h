//
// Metal图形层Buffer
//

#pragma once

#include "GfxDevice/GfxBuffer.h"
#include "Container/Vector.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

class GfxBufferMetal : public GfxBuffer
{
	OBJECT_OVERRIDE(GfxBufferMetal, GfxBuffer);
public:
	explicit GfxBufferMetal();

	~GfxBufferMetal() override;

	// 应用当前设置（未调用之前buffer处于不可用状态）
	void Apply(const void* initialDataPtr) override;

	// 更新buffer的数据（usage == BUFFER_DYANMIC时可用）
	void UpdateBuffer(const void* dataPtr) override;

	// 更新buffer一个范围内的数据（usage == BUFFER_DYANMIC时可用）
	void UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size) override;

	// 开始写数据
	void* BeginWrite(UInt32 offset, UInt32 size) override;

	// 结束写数据
	void EndWrite(UInt32 bytesWritten) override;

	// 获取CPU映射数据
	const UInt8* GetShadowData() const override { return shadowdData_.Buffer(); }

private:
	mtlpp::Buffer mtlBuffer_;

	UInt32 lastWriteBeginOffset_;

	PODVector<UInt8> shadowdData_;
};

}
