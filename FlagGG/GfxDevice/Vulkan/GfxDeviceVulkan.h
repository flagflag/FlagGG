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

class GfxSampler;
class GfxShaderVulkan;
class GfxProgramVulkan;
class VulkanDynamicUniformBuffer;
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
	VkQueue GetGraphicsQueue() { return vkGraphicsQueue_; }

	//
	VkCommandBuffer GetVulkanCmdBuffer() { return vkCmdBuffer_; }

	//
	uint32_t GetVulkanMemoryTypeIndex(uint32_t vkMemoryTypeBits, VkMemoryPropertyFlags vkMemPropFlags) const;

	//
	VkCommandBuffer BeginCommandBuffer(VkCommandBufferUsageFlagBits vkCmdBufferUsageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//
	void EndCommandBuffer(VkCommandBuffer vkCmdBuffer, bool waitForFinish);

	//
	void FlushCommandBuffer();

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

	void PrepareColorBlendState(VkPipelineColorBlendStateCreateInfo& vkPCBSCI);

	void PrepareDynamicState(VkPipelineDynamicStateCreateInfo& vkPDSCI);

	VkPipeline CreateRenderPipelineFromCurrentState();

	void PrepareComputePipelineState();

	VkSampler GetSampler(GfxSampler* gfxSampler);

	void BindTextures(VkDescriptorSet vkDescSet, GfxShaderVulkan* shaderVulkan);

	void SetShaderParameters(VkDescriptorSet vkDescSet, GfxShaderVulkan* shaderVulkan);

private:
	// 当前选择的物理设备
	VkPhysicalDevice vkPhysicalDevice_;

	// vulkan实例
	VkInstance vkInstance_;

	// Vulkan设备
	VkDevice vkDevice_;

	// 内存分配器
	VkAllocationCallbacks vkAllocCallback_;

	// 调试
	VkDebugReportCallbackEXT debugReportCallback_;

	// 物理设备的内存属性
	VkPhysicalDeviceMemoryProperties vkPhyDvMemProp_;

	// 物理设备可用features
	VkPhysicalDeviceFeatures vkPhyDvFeatures_;

	// 命令buffer内存池
	VkCommandPool vkCmdPool_;

	VkQueue vkGraphicsQueue_;

	VkQueue vkComputeQueue_;

	// 当前的命令buffer
	VkCommandBuffer vkCmdBuffer_;

	VkCommandBuffer vkCmdBuffers_[4];

	// 描述集合内存池
	VkDescriptorPool vkDescPool_;

	// 所有Pipeline的缓存
	VkPipelineCache vkAllPipelineCache_;

	// compute管线
	VkPipeline vkComputePipeline_;

	// 光栅化管线
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
	// RenderPass和Framebuffer的缓存
	HashMap<VulkanRenderPassAttachmentsKey, VulkanRenderPassInfo> vkRenderPassMap_;

	bool vkRenderPassDirty_;

	// 管线缓存
	HashMap<UInt32, VkPipeline> vkPipelineMap_;

	//
	PODVector<VkDescriptorSet> vkDescSets_;

	//
	PODVector<VkDescriptorImageInfo> vkDescImageInfos_;

	//
	PODVector<VkDescriptorBufferInfo> vkDescBufferInfos_;

	//
	PODVector<uint32_t> uniformBufferOffset_;

	//
	PODVector<VkWriteDescriptorSet> vkWriteDescSets_;

	// VS + PS => Program
	HashMap<Pair<WeakPtr<GfxShader>, WeakPtr<GfxShader>>, SharedPtr<GfxProgramVulkan>> programMap_;

	// 当前使用的program
	SharedPtr<GfxProgramVulkan> currentProgram_;

	// 采样器缓存
	HashMap<UInt32, VkSampler> samplerStateMap_;

	// 当前使用的UniformBuffer
	SharedPtr<VulkanDynamicUniformBuffer> currentUniformBuffer_;
};

}
