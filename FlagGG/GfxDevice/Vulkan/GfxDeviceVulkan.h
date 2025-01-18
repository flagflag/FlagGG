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

class GfxShaderVulkan;
class GfxProgramVulkan;
struct VulkanConstanceBufferDesc;

struct VulkanRenderPassAttachmentsKey
{
	VulkanRenderPassAttachmentsKey()
		: hashValue_(0u)
		, id_{}
	{

	}

	VulkanRenderPassAttachmentsKey(GfxRenderSurface* surface1, GfxRenderSurface* surface2, GfxRenderSurface* surface3, GfxRenderSurface* surface4, GfxRenderSurface* surface5)
		: id_{ UInt64(surface1), UInt64(surface2), UInt64(surface3), UInt64(surface4), UInt64(surface5), }
		, hashValue_(0u)
	{}

	bool operator==(const VulkanRenderPassAttachmentsKey& rhs) const
	{
		for (unsigned i = 0; i < MAX_RENDERTARGET_COUNT + 1; ++i)
		{
			if (id_[i] != rhs.id_[i])
				return false;
		}
		return true;
	}

	inline UInt64& operator[](unsigned idx)
	{
		return id_[idx];
	}

	void CalculateHash()
	{
		hashValue_ = 0u;
		UInt32 num = (MAX_RENDERTARGET_COUNT + 1u) * 8u;
		const char* ptr = reinterpret_cast<const char*>(id_);
		while (num--)
		{
			hashValue_ = *ptr + (hashValue_ << 6u) + (hashValue_ << 16u) - hashValue_;
			++ptr;
		}
	}

	inline UInt32 ToHash() const
	{
		return hashValue_;
	}

	UInt32 hashValue_;
	UInt64 id_[MAX_RENDERTARGET_COUNT + 1];
};

class GfxDeviceVulkan : public GfxDevice, public Subsystem<GfxDeviceVulkan>
{
public:
	explicit GfxDeviceVulkan();

	~GfxDeviceVulkan() override;

	/**********************************************************/
	/*                        渲染指令                        */
	/**********************************************************/

	// 帧开始
	void BeginFrame() override;

	// 帧结束
	void EndFrame() override;

	// 开始Pass
	void BeginPass(const char* renderPassName) override;

	// 结束Pass
	void EndPass() override;

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

	// 获取vulkan物理设备
	VkPhysicalDevice GetVulkanPhysicalDevice() { return vkPhysicalDevice_; }

	// 获取vulkan instnace
	VkInstance GetVulkanInstance() { return vkInstance_; }

	// 获取vukan device
	VkDevice GetVulkanDevice() { return vkDevice_; }

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

	VkCommandBuffer BeginNewRenderCommand(VkRenderPass vkRenderPass, VkFramebuffer vkFramebuffer);

	void EndRenderCommand(VkCommandBuffer vkCmdBuffer);

	VkRenderPass CreateRenderPassFromCurrentState();

	VkFramebuffer CreateFramebufferFromCurrentState(VkRenderPass vkRenderPass);

	void PrepareRenderPass();

	void PrepareRenderPipelineState();

	void PrepareShaderStage(GfxShaderVulkan* shader, VkShaderStageFlagBits shaderStage, VkPipelineShaderStageCreateInfo& vkPSSCI);

	void PrepareVertexInputState(VertexDescription* vertxDesc, VkPipelineVertexInputStateCreateInfo& vkPVISCI);

	void PrepareInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& vkPIASCI);

	void PrepareViewportState(VkPipelineViewportStateCreateInfo& vkPVSCI);

	void PrepareRasterizationState(VkPipelineRasterizationStateCreateInfo& vkPRSCI);

	void PrepareMultisampleState(VkPipelineMultisampleStateCreateInfo& vkPMSCI);

	void PrepareDepthStencilState(VkPipelineDepthStencilStateCreateInfo& vkPDSSCI);

	void PrepareColorBlendState(VkPipelineColorBlendStateCreateInfo& vkPCBSCI, VkPipelineColorBlendAttachmentState& vkPCBAS);

	void PrepareDynamicState(VkPipelineDynamicStateCreateInfo& vkPDSCI);

	VkPipeline CreateRenderPipelineFromCurrentState();

	void PrepareComputePipelineState();

	void CopyShaderParameterToBuffer(const HashMap<UInt32, VulkanConstanceBufferDesc>& bufferDesc, GfxBuffer* bufferArray);

private:
	// 当前选择的物理设备
	VkPhysicalDevice vkPhysicalDevice_;

	VkInstance vkInstance_;

	VkDevice vkDevice_;

	VkAllocationCallbacks vkAllocCallback_;

	VkDebugReportCallbackEXT debugReportCallback_;

	VkPhysicalDeviceMemoryProperties vkPhyDvMemProp_;

	VkCommandPool vkCmdPool_;

	VkQueue vkGraphicsQueue_;

	VkQueue vkComputeQueue_;

	VkCommandBuffer vkCmdBuffer_;

	VkCommandBuffer vkCmdBuffers_[4];

	VkPipelineCache vkAllPipelineCache_;

	VkPipeline vkComputePipeline_;

	VkPipeline vkGraphicsPipeline_;

	// 当前执行的RenderPass
	VkRenderPass vkRenderPass_;

	// 当前绑定的Framebuffer
	VkFramebuffer vkFramebuffer_;

	struct VulkanRenderPassInfo
	{
		VkRenderPass vkRenderPass_;
		VkFramebuffer vkFramebuffer_;
	};
	HashMap<VulkanRenderPassAttachmentsKey, VulkanRenderPassInfo> vkRenderPassMap_;

	bool vkRenderPassDirty_;

	HashMap<UInt32, VkPipeline> vkPipelineMap_;

	HashMap<Pair<WeakPtr<GfxShader>, WeakPtr<GfxShader>>, SharedPtr<GfxProgramVulkan>> programMap_;
	SharedPtr<GfxProgramVulkan> currentProgram_;

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
