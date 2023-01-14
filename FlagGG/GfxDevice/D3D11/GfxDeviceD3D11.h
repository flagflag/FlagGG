//
// D3D11图形层设备
//

#pragma once

#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/D3D11/GfxBufferD3D11.h"
#include "Container/HashMap.h"
#include "Utility/Singleton.h"
#include "AsyncFrame/Mutex.h"
#include "Math/Math.h"
#include "Math/Matrix3x4.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxShaderD3D11;
class GfxBufferD3D11;
class GfxSampler;

class GfxDeviceD3D11 : public GfxDevice, public Singleton<GfxDeviceD3D11, NullMutex>
{
public:
	explicit GfxDeviceD3D11();

	~GfxDeviceD3D11() override;

	/**********************************************************/
	/*                        渲染指令                        */
	/**********************************************************/

	// 清理RenderTarget、DepthStencil
	void Clear(ClearTargetFlags flags, const Color& color = Color::TRANSPARENT_BLACK, float depth = 1.0f, unsigned stencil = 0) override;

	// 提交渲染指令
	void Draw(UInt32 vertexStart, UInt32 vertexCount) override;

	// 提交渲染指令
	void DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart) override;

	// Flush
	void Flush() override;


	/**********************************************************/
	/*                     创建Gfx对象                        */
	/**********************************************************/

	// 创建交换链
	GfxSwapChain* CreateSwapChain(Window* window) override;

	// 创建纹理
	GfxTexture* CreateTexture() override;

	// 创建buffer
	GfxBuffer* CreateBuffer() override;

	// 创建shader
	GfxShader* CreateShader() override;

	// 创建gpu program
	GfxProgram* CreateProgram() override;


	ID3D11Device* GetD3D11Device() const
	{
		return device_;
	}

	ID3D11DeviceContext* GetD3D11DeviceContext() const
	{
		return deviceContext_;
	}

	bool CheckMultiSampleSupport(DXGI_FORMAT format, UInt32 sampleCount);

	UInt32 GetMultiSampleQuality(DXGI_FORMAT format, UInt32 sampleCount);

protected:
	// 提交渲染指令之前预处理工作
	void PrepareDraw();

	void PrepareRasterizerState();

	void CopyShaderParameterToBuffer(GfxShaderD3D11* shader, GfxBufferD3D11* buffer);

	ID3D11InputLayout* GetD3D11InputLayout(VertexDescription* verteDesc, GfxShaderD3D11* vertexShader);

	ID3D11SamplerState* GetD3D11SamplerState(GfxSampler* gfxSampler);

private:
	// Constant buffer用途
	enum ConstBufferType
	{
		CONST_BUFFER_WORLD = 0,
		CONST_BUFFER_SKIN,
		CONST_BUFFER_VS,
		CONST_BUFFER_PS,
		MAX_CONST_BUFFER,
	};

	// D3D11设备
	ID3D11Device* device_{};

	// D3D11设备上下文
	ID3D11DeviceContext* deviceContext_{};

	// 光栅化状态缓存
	HashMap<UInt32, ID3D11RasterizerState*> rasterizerStates_;

	// uniform
	GfxBufferD3D11 vsConstantBuffer_[MAX_CONST_BUFFER_COUNT];
	GfxBufferD3D11 psConstantBuffer_[MAX_CONST_BUFFER_COUNT];

	HashMap<Pair<UInt32, GfxShaderD3D11*>, ID3D11InputLayout*> d3d11InputLayoutMap_;

	HashMap<UInt32, ID3D11SamplerState*> d3d11SamplerStateMap_;
};

}
