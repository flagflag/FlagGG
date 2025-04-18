//
// D3D11图形层Buffer
//

#pragma once

#include "GfxDevice/GfxBuffer.h"
#include "Container/Vector.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxBufferD3D11 : public GfxBuffer
{
	OBJECT_OVERRIDE(GfxBufferD3D11, GfxBuffer);
public:
	explicit GfxBufferD3D11();

	~GfxBufferD3D11() override;

	// 设置Gpu tag
	void SetGpuTag(const String& gpuTag) override;

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

	//
	ID3D11Buffer* GetD3D11Buffer() const { return d3d11Buffer_; }

	// 获取ID3D11ShaderResourceView*
	ID3D11ShaderResourceView* GetD3D11ShaderResourceView() const { return d3d11SRV_; }

	// 获取ID3D11UnorderedAccessView*
	ID3D11UnorderedAccessView* GetD3D11UnorderedAccessView() const { return d3d11UAV_; }

private:
	ID3D11Buffer* d3d11Buffer_{};

	ID3D11ShaderResourceView* d3d11SRV_{};

	ID3D11UnorderedAccessView* d3d11UAV_{};

	PODVector<UInt8> shadowdData_;
};

}
