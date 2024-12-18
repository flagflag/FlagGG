//
// Metal图形层设备
//

#pragma once

#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/Metal/GfxBufferMetal.h"
#include "Core/Subsystem.h"
#include "Container/HashMap.h"
#include "Utility/Singleton.h"
#include "AsyncFrame/Mutex.h"
#include "Math/Math.h"
#include "Math/Matrix3x4.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

class GfxDeviceMetal : public GfxDevice, public Subsystem<GfxDeviceMetal>
{
public:
	explicit GfxDeviceMetal();

	~GfxDeviceMetal() override;

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

	// 获取metal设备
	mtlpp::Device& GetMetalDevice();

protected:
	// 提交渲染指令之前预处理工作
	void PrepareDraw();

	void PrepareRasterizerState();

	void PrepareDepthStencilState();

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

	// uniform
	GfxBufferMetal vsConstantBuffer_[MAX_CONST_BUFFER_COUNT];
	GfxBufferMetal psConstantBuffer_[MAX_CONST_BUFFER_COUNT];
};

}
