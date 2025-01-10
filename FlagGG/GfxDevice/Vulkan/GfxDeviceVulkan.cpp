#include "GfxDeviceVulkan.h"
#include "GfxSwapChainVulkan.h"
#include "GfxTextureVulkan.h"
#include "GfxBufferVulkan.h"
#include "GfxShaderVulkan.h"
#include "GfxProgramVulkan.h"
#include "VulkanDefines.h"
#include "GfxDevice/VertexDescFactory.h"
#include "Memory/MemoryHook.h"

namespace FlagGG
{

static const VkCullModeFlagBits vulkanCullMode[] =
{
	VK_CULL_MODE_NONE,
	VK_CULL_MODE_FRONT_BIT,
	VK_CULL_MODE_BACK_BIT,
};

static const VkPolygonMode vulkanFillMode[] =
{
	VK_POLYGON_MODE_LINE,
	VK_POLYGON_MODE_FILL,
};

static const VkBlendFactor vulkanSourceRgbBlendFactor[] =
{
	VK_BLEND_FACTOR_ZERO,
};

static const VkBlendFactor vulkanDestRgbBlendFactor[] =
{
	VK_BLEND_FACTOR_ZERO,
};

static const VkBlendOp vulkanRgbBlendOpt[] =
{
	VK_BLEND_OP_ADD,
};

static const VkBlendFactor vulkanSourceAlphaBlendFactor[] =
{
	VK_BLEND_FACTOR_ZERO,
};

static const VkBlendFactor vulkanDestAlphaBlendFactor[] =
{
	VK_BLEND_FACTOR_ZERO,
};

static const VkBlendOp vulkanAlphaBlendOpt[] =
{
	VK_BLEND_OP_ADD,
};

static const VkCompareOp vulkanCompareFunction[] =
{
	VK_COMPARE_OP_NEVER,
	VK_COMPARE_OP_LESS,
	VK_COMPARE_OP_EQUAL,
	VK_COMPARE_OP_LESS_OR_EQUAL,
	VK_COMPARE_OP_GREATER,
	VK_COMPARE_OP_NOT_EQUAL,
	VK_COMPARE_OP_GREATER_OR_EQUAL,
	VK_COMPARE_OP_ALWAYS,
};

static const VkStencilOp vulkanStencilOperation[] =
{
	VK_STENCIL_OP_KEEP,
	VK_STENCIL_OP_ZERO,
	VK_STENCIL_OP_REPLACE,
	VK_STENCIL_OP_INCREMENT_AND_CLAMP,
	VK_STENCIL_OP_DECREMENT_AND_CLAMP,
	VK_STENCIL_OP_INVERT,
	VK_STENCIL_OP_INCREMENT_AND_WRAP,
	VK_STENCIL_OP_DECREMENT_AND_WRAP,
};

static const VkPrimitiveTopology vulkanPrimitiveType[] =
{
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
};

GfxDeviceVulkan::GfxDeviceVulkan()
{
	vkAllocCallback_.pUserData = this;
	vkAllocCallback_.pfnAllocation = [](void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) -> void*
	{
		return GetSubsystem<Memory>()->Malloc(size, alignment);
	};
	vkAllocCallback_.pfnFree = [](void* pUserData, void* pMemory)
	{
		return GetSubsystem<Memory>()->Free(pMemory);
	};
	vkAllocCallback_.pfnReallocation = [](void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) -> void*
	{
		return GetSubsystem<Memory>()->Realloc(pOriginal, size, alignment);
	};
	vkAllocCallback_.pfnInternalAllocation = [](void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
	{
		
	};
	vkAllocCallback_.pfnInternalFree = [](void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
	{

	};

	VkApplicationInfo vkAppInfo;
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pNext = nullptr;
	vkAppInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	vkAppInfo.applicationVersion = 0;
	vkAppInfo.engineVersion = 0;
	vkAppInfo.pApplicationName = "FlagGG";
	vkAppInfo.pEngineName = "FlagGG";

	VkInstanceCreateInfo vkICI;
	vkICI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkICI.pNext = nullptr;
	vkICI.enabledExtensionCount = 0;
	vkICI.enabledLayerCount = 0;
	vkICI.flags = 0;
	vkICI.pApplicationInfo = &vkAppInfo;
	vkICI.ppEnabledExtensionNames = nullptr;
	vkICI.ppEnabledLayerNames = nullptr;
	VULKAN_CHECK(vkCreateInstance(&vkICI, &vkAllocCallback_, &vkInstance_));

	uint32_t numPhysicalDevices = 0;
	VULKAN_CHECK(vkEnumeratePhysicalDevices(vkInstance_, &numPhysicalDevices, nullptr));
	PODVector<VkPhysicalDevice> physicalDevices;
	physicalDevices.Resize(numPhysicalDevices);
	VULKAN_CHECK(vkEnumeratePhysicalDevices(vkInstance_, &numPhysicalDevices, &physicalDevices[0]));

	VkPhysicalDevice selectPhysicalDevice = nullptr;
	for (UInt32 i = 0; i < numPhysicalDevices; ++i)
	{
		selectPhysicalDevice = physicalDevices[i];
	}

	vkGetPhysicalDeviceMemoryProperties(selectPhysicalDevice, &vkPhyDvMemProp_);

	uint32_t numQueueFamilyPropertices;
	vkGetPhysicalDeviceQueueFamilyProperties(selectPhysicalDevice, &numQueueFamilyPropertices, nullptr);
	PODVector<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.Resize(numQueueFamilyPropertices);
	vkGetPhysicalDeviceQueueFamilyProperties(selectPhysicalDevice, &numQueueFamilyPropertices, &queueFamilyProperties[0]);
	uint32_t graphicsQueueIndex = 0;
	uint32_t computeQueueIndex = 0;
	for (uint32_t i = 0; i < numQueueFamilyPropertices; ++i)
	{
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueIndex = i;
		}
		else if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			computeQueueIndex = i;
		}
	}

	const float queuePriorities[1] = { 0.0f };
	PODVector<VkDeviceQueueCreateInfo> vkDQCI(2);
	for (auto& it : vkDQCI)
	{
		it.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		it.pNext = nullptr;
		it.flags = 0;
		it.pQueuePriorities = queuePriorities;
		it.queueCount = 1;
	}
	vkDQCI[0].queueFamilyIndex = graphicsQueueIndex;
	vkDQCI[1].queueFamilyIndex = computeQueueIndex;

	VkDeviceCreateInfo vkDCI;
	vkDCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDCI.pNext = nullptr;
	vkDCI.enabledExtensionCount = 0;
	vkDCI.enabledLayerCount = 0;
	vkDCI.flags = 0;
	vkDCI.pEnabledFeatures = nullptr;
	vkDCI.ppEnabledExtensionNames = nullptr;
	vkDCI.ppEnabledLayerNames = nullptr;
	vkDCI.pQueueCreateInfos = &vkDQCI[0];
	vkDCI.queueCreateInfoCount = 2;
	VULKAN_CHECK(vkCreateDevice(selectPhysicalDevice, &vkDCI, &vkAllocCallback_, &vkDevice_));

	vkGetDeviceQueue(vkDevice_, graphicsQueueIndex, 0, &vkGraphicsQueue_);
	vkGetDeviceQueue(vkDevice_, computeQueueIndex, 0, &vkComputeQueue_);

	VkCommandPoolCreateInfo vkCPCI;
	vkCPCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vkCPCI.pNext = nullptr;
	vkCPCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCPCI.queueFamilyIndex = graphicsQueueIndex;
	VULKAN_CHECK(vkCreateCommandPool(vkDevice_, &vkCPCI, &vkAllocCallback_, &vkCmdPool_));
}

GfxDeviceVulkan::~GfxDeviceVulkan()
{
	vkDestroyCommandPool(vkDevice_, vkCmdPool_, &vkAllocCallback_);
	vkDestroyDevice(vkDevice_, &vkAllocCallback_);
	vkDestroyInstance(vkInstance_, &vkAllocCallback_);
}

void GfxDeviceVulkan::Clear(ClearTargetFlags flags, const Color& color, float depth, unsigned stencil)
{
	if (flags & CLEAR_COLOR)
	{
		// vkCmdClearColorImage(vkCmdBuffer_, );
	}

	if (flags & (CLEAR_DEPTH | CLEAR_STENCIL))
	{
		// vkCmdClearDepthStencilImage(vkCmdBuffer_, );
	}
}

void GfxDeviceVulkan::Draw(UInt32 vertexStart, UInt32 vertexCount)
{
	PrepareDraw();

	vkCmdDraw(vkCmdBuffer_, vertexCount, 1, vertexStart, 0);
}

void GfxDeviceVulkan::DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart)
{
	PrepareDraw();

	vkCmdDrawIndexed(vkCmdBuffer_, indexCount, 1, indexStart, vertexStart, 0);
}

void GfxDeviceVulkan::DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount)
{
	PrepareDraw();

	vkCmdDrawIndexed(vkCmdBuffer_, indexCount, instanceCount, indexStart, vertexStart, 0);
}

void GfxDeviceVulkan::Flush()
{

}

GfxSwapChain* GfxDeviceVulkan::CreateSwapChain(Window* window)
{
	return new GfxSwapChainVulkan(window);
}

GfxTexture* GfxDeviceVulkan::CreateTexture()
{
	return new GfxTextureVulkan();
}

GfxBuffer* GfxDeviceVulkan::CreateBuffer()
{
	return new GfxBufferVulkan();
}

GfxShader* GfxDeviceVulkan::CreateShader()
{
	return new GfxShaderVulkan();
}

GfxProgram* GfxDeviceVulkan::CreateProgram()
{
	return new GfxProgramVulkan();
}

void GfxDeviceVulkan::PrepareDraw()
{
	PrepareRenderPassAttachments();

	PrepareRenderPiplineState();

	PrepareDepthStencilState();

	if (vertexBufferDirty_)
	{
		static VkBuffer vulkanVertexBuffers[MAX_VERTEX_BUFFER_COUNT] = {};
		static VkDeviceSize vulkanVertexOffset[MAX_VERTEX_BUFFER_COUNT] = {};

		const UInt32 vulkanVertexBufferCount = Min<UInt32>(vertexBuffers_.Size(), MAX_VERTEX_BUFFER_COUNT);

		for (UInt32 i = 0; i < vulkanVertexBufferCount; ++i)
		{
			auto* vertexBufferVulkan = RTTICast<GfxBufferVulkan>(vertexBuffers_[i]);
			vulkanVertexBuffers[i] = vertexBufferVulkan->GetVulkanBuffer();
			vulkanVertexOffset[i] = 0;
		}

		vkCmdBindVertexBuffers(vkCmdBuffer_, 0, vulkanVertexBufferCount, vulkanVertexBuffers, nullptr);

		vertexBufferDirty_ = false;
	}

	if (indexBufferDirty_)
	{
		auto* indexBufferVulkan = RTTICast<GfxBufferVulkan>(indexBuffer_);
		vkCmdBindIndexBuffer(vkCmdBuffer_, indexBufferVulkan->GetVulkanBuffer(), 0,
			indexBuffer_->GetDesc().stride_ == sizeof(UInt16) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

		indexBufferDirty_ = false;
	}

	if (texturesDirty_ || samplerDirty_)
	{

		texturesDirty_ = false;
	}
}

void GfxDeviceVulkan::PrepareRenderPassAttachments()
{
	if (renderTargetDirty_ || depthStencilDirty_)
	{

		renderTargetDirty_ = false;
		depthStencilDirty_ = false;
	}
}

void GfxDeviceVulkan::PrepareRenderPiplineState()
{
	if (rasterizerStateDirty_)
	{
		// vkCmdSetCullModeEXT(vkCmdBuffer_, vulkanCullMode[rasterizerState_.cullMode_]);
		// vkCmdSetFrontFaceEXT(vkCmdBuffer_, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		vkCmdSetDepthBias(vkCmdBuffer_, rasterizerState_.depthBias_, F_INFINITY, rasterizerState_.slopeScaledDepthBias_);
	}

	if (rasterizerStateDirty_ || shaderDirty_)
	{


		rasterizerStateDirty_ = false;
		shaderDirty_ = false;
	}
}

void GfxDeviceVulkan::PrepareDepthStencilState()
{
	if (depthStencilStateDirty_)
	{
		if (depthStencilState_.depthTestMode_ != COMPARISON_ALWAYS)
		{
			// vkCmdSetDepthTestEnableEXT(vkCmdBuffer_, VK_TRUE);
			// vkCmdSetDepthCompareOpEXT(vkCmdBuffer_, vulkanCompareFunction[depthStencilState_.depthTestMode_]);
		}
		else
		{
			// vkCmdSetDepthTestEnableEXT(vkCmdBuffer_, VK_FALSE);
		}
		// vkCmdSetDepthWriteEnableEXT(vkCmdBuffer_, depthStencilState_.depthWrite_ ? VK_TRUE : VK_FALSE);

		// vkCmdSetStencilTestEnableEXT(vkCmdBuffer_, depthStencilState_.stencilTest_ ? VK_TRUE : VK_FALSE);
		vkCmdSetStencilReference(vkCmdBuffer_, VK_STENCIL_FACE_FRONT_BIT, depthStencilState_.stencilRef_);
		vkCmdSetStencilCompareMask(vkCmdBuffer_, VK_STENCIL_FACE_FRONT_BIT, depthStencilState_.stencilReadMask_);
		vkCmdSetStencilWriteMask(vkCmdBuffer_, VK_STENCIL_FACE_FRONT_BIT, depthStencilState_.stencilWriteMask_);
		//vkCmdSetStencilOpEXT(vkCmdBuffer_, VK_STENCIL_FACE_FRONT_BIT,
		//	vulkanStencilOperation[depthStencilState_.stencilFailOp_],
		//	vulkanStencilOperation[depthStencilState_.depthStencilPassOp_],
		//	vulkanStencilOperation[depthStencilState_.depthFailOp_],
		//	vulkanCompareFunction[depthStencilState_.stencilTestMode_]);

		depthStencilStateDirty_ = false;
	}
}

void GfxDeviceVulkan::CopyShaderParameterToBuffer(const HashMap<UInt32, VulkanConstanceBufferDesc>& bufferDesc, GfxBuffer* bufferArray)
{

}

uint32_t GfxDeviceVulkan::GetVulkanMemoryTypeIndex(uint32_t vkMemoryTypeBits, VkMemoryPropertyFlags vkMemPropFlags) const
{
	for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < vkPhyDvMemProp_.memoryTypeCount; ++memoryTypeIndex)
	{
		if (((1 << memoryTypeIndex) & vkMemoryTypeBits) != 0 &&
			(vkPhyDvMemProp_.memoryTypes[memoryTypeIndex].propertyFlags & vkMemPropFlags) == vkMemPropFlags)
		{
			return memoryTypeIndex;
		}
	}

	ASSERT(false);
	return -1;
}

VkCommandBuffer GfxDeviceVulkan::BeginCommandBuffer(VkCommandBufferUsageFlagBits vkCmdBufferUsageFlags)
{
	VkCommandBuffer vkCmdBuffer;
	VkCommandBufferAllocateInfo vkCBAI;
	vkCBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCBAI.pNext = nullptr;
	vkCBAI.commandBufferCount = 1;
	vkCBAI.commandPool = vkCmdPool_;
	vkCBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	VULKAN_CHECK(vkAllocateCommandBuffers(vkDevice_, &vkCBAI, &vkCmdBuffer), VK_NULL_HANDLE);

	VkCommandBufferBeginInfo vkCBBI;
	vkCBBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCBBI.pNext = nullptr;
	vkCBBI.flags = vkCmdBufferUsageFlags;
	vkCBBI.pInheritanceInfo = nullptr;
	VULKAN_CHECK(vkBeginCommandBuffer(vkCmdBuffer, &vkCBBI), VK_NULL_HANDLE);

	return vkCmdBuffer;
}

void GfxDeviceVulkan::EndCommandBuffer(VkCommandBuffer vkCmdBuffer, bool waitForFinish)
{
	VULKAN_CHECK(vkEndCommandBuffer(vkCmdBuffer));

	VkSubmitInfo vkSI;
	vkSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSI.pNext = nullptr;
	vkSI.commandBufferCount = 1;
	vkSI.pCommandBuffers = &vkCmdBuffer;
	vkSI.pSignalSemaphores = nullptr;
	vkSI.pWaitDstStageMask = nullptr;
	vkSI.pWaitSemaphores = nullptr;
	vkSI.signalSemaphoreCount = 0;
	vkSI.waitSemaphoreCount = 0;
	VULKAN_CHECK(vkQueueSubmit(vkGraphicsQueue_, 1, &vkSI, VK_NULL_HANDLE));
	if (waitForFinish)
	{
		VULKAN_CHECK(vkQueueWaitIdle(vkGraphicsQueue_));
	}

	vkFreeCommandBuffers(vkDevice_, vkCmdPool_, 1, &vkCmdBuffer);
}

}
