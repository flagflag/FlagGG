//
// Vulkan图形层设备
//

#pragma once

#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/Vulkan/GfxBufferVulkan.h"
#include "Core/Subsystem.h"
#include "Container/HashMap.h"
#include "Utility/Singleton.h"
#include "AsyncFrame/Mutex.h"
#include "Math/Math.h"
#include "Math/Matrix3x4.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxProgramVulkan;
struct VulkanConstanceBufferDesc;

class GfxDeviceVulkan : public GfxDevice, public Subsystem<GfxDeviceVulkan>
{
public:
	explicit GfxDeviceVulkan();

	~GfxDeviceVulkan() override;

	/**********************************************************/
	/*                        渲染指令                        */
	/**********************************************************/

	// 清理RenderTarget、DepthStencil
	void Clear(ClearTargetFlags flags, const Color& color = Color::TRANSPARENT_BLACK, float depth = 1.0f, unsigned stencil = 0) override;

	// 提交渲染指令
	void Draw(UInt32 vertexStart, UInt32 vertexCount) override;

	// 提交渲染指令
	void DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart) override;

	// 提交渲染指令
	void DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount) override;

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

	// 获取vukan device
	VkDevice& GetVulkanDevice() { return vkDevice_; }

	// 获取vulkan alloction callback
	VkAllocationCallbacks& GetVulkanAllocCallback() { return vkAllocCallback_; }

	// 获取设备内存属性
	VkPhysicalDeviceMemoryProperties& GetVulkanPhyDvMemProp() { return vkPhyDvMemProp_; }

	//
	uint32_t GetVulkanMemoryTypeIndex(uint32_t vkMemoryTypeBits, VkMemoryPropertyFlags vkMemPropFlags) const;

	//
	VkCommandBuffer BeginCommandBuffer(VkCommandBufferUsageFlagBits vkCmdBufferUsageFlags);

	//
	void EndCommandBuffer(VkCommandBuffer vkCmdBuffer, bool waitForFinish);

protected:
	// 提交渲染指令之前预处理工作
	void PrepareDraw();

	void PrepareRenderPassAttachments();

	void PrepareRenderPiplineState();

	void PrepareDepthStencilState();

	void CopyShaderParameterToBuffer(const HashMap<UInt32, VulkanConstanceBufferDesc>& bufferDesc, GfxBuffer* bufferArray);

private:
	VkInstance vkInstance_;

	VkDevice vkDevice_;

	VkAllocationCallbacks vkAllocCallback_;

	VkPhysicalDeviceMemoryProperties vkPhyDvMemProp_;

	VkCommandPool vkCmdPool_;

	VkQueue vkGraphicsQueue_;

	VkQueue vkComputeQueue_;

	VkCommandBuffer vkCmdBuffer_;

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
	GfxBufferVulkan vsConstantBuffer_[MAX_CONST_BUFFER_COUNT];
	GfxBufferVulkan psConstantBuffer_[MAX_CONST_BUFFER_COUNT];
};

}
