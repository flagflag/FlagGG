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

static const VkFormat vulkanElementFormats[] =
{
	VK_FORMAT_R32_SINT,               // DXGI_FORMAT_R32_SINT,
	VK_FORMAT_R32_SFLOAT,             // DXGI_FORMAT_R32_FLOAT,
	VK_FORMAT_R32G32_SFLOAT,          // DXGI_FORMAT_R32G32_FLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,       // DXGI_FORMAT_R32G32B32_FLOAT,
	VK_FORMAT_R32G32B32A32_SFLOAT,    // DXGI_FORMAT_R32G32B32A32_FLOAT,
	VK_FORMAT_R8G8B8A8_UINT,          // DXGI_FORMAT_R8G8B8A8_UINT,
	VK_FORMAT_R8G8B8A8_UNORM,         // DXGI_FORMAT_R8G8B8A8_UNORM
};

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
	: vkRenderPassDirty_(false)
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
	vkAppInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pNext              = nullptr;
	vkAppInfo.apiVersion         = VK_MAKE_VERSION(1, 0, 0);
	vkAppInfo.applicationVersion = 0;
	vkAppInfo.engineVersion      = 0;
	vkAppInfo.pApplicationName   = "FlagGG";
	vkAppInfo.pEngineName        = "FlagGG";

	VkInstanceCreateInfo vkICI;
	vkICI.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkICI.pNext                   = nullptr;
	vkICI.enabledExtensionCount   = 0;
	vkICI.enabledLayerCount       = 0;
	vkICI.flags                   = 0;
	vkICI.pApplicationInfo        = &vkAppInfo;
	vkICI.ppEnabledExtensionNames = nullptr;
	vkICI.ppEnabledLayerNames     = nullptr;
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
	vkDCI.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDCI.pNext                   = nullptr;
	vkDCI.enabledExtensionCount   = 0;
	vkDCI.enabledLayerCount       = 0;
	vkDCI.flags                   = 0;
	vkDCI.pEnabledFeatures        = nullptr;
	vkDCI.ppEnabledExtensionNames = nullptr;
	vkDCI.ppEnabledLayerNames     = nullptr;
	vkDCI.pQueueCreateInfos       = &vkDQCI[0];
	vkDCI.queueCreateInfoCount    = 2;
	VULKAN_CHECK(vkCreateDevice(selectPhysicalDevice, &vkDCI, &vkAllocCallback_, &vkDevice_));

	vkGetDeviceQueue(vkDevice_, graphicsQueueIndex, 0, &vkGraphicsQueue_);
	vkGetDeviceQueue(vkDevice_, computeQueueIndex, 0, &vkComputeQueue_);

	VkCommandPoolCreateInfo vkCPCI;
	vkCPCI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vkCPCI.pNext            = nullptr;
	vkCPCI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCPCI.queueFamilyIndex = graphicsQueueIndex;
	VULKAN_CHECK(vkCreateCommandPool(vkDevice_, &vkCPCI, &vkAllocCallback_, &vkCmdPool_));

	VkPipelineCacheCreateInfo vkPCCI;
	vkPCCI.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkPCCI.pNext           = nullptr;
	vkPCCI.flags           = 0;
	vkPCCI.initialDataSize = 0;
	vkPCCI.pInitialData    = nullptr;
	VULKAN_CHECK(vkCreatePipelineCache(vkDevice_, &vkPCCI, &vkAllocCallback_, &vkAllPipelineCache_));
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

	PrepareRenderPipelineState();

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
		VulkanRenderPassAttachmentsKey key(renderTargets_[0], renderTargets_[1], renderTargets_[2], renderTargets_[3], depthStencil_);
		auto it = vkRenderPassMap_.Find(key);
		if (it == vkRenderPassMap_.End())
		{
			VkRenderPassCreateInfo vkRPCI;
			vkRPCI.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vkRPCI.pNext           = nullptr;

			VkAttachmentDescription vkAttachmentDescArray[MAX_RENDERTARGET_COUNT + 1];
			VkAttachmentReference vkColorAttachmentRef[MAX_RENDERTARGET_COUNT];
			VkAttachmentReference vkResolveAttachmentRef[MAX_RENDERTARGET_COUNT + 1];
			VkAttachmentReference vkDepthStencilAttachmentRef;
			UInt32 colorAttachmentCount = 0;
			UInt32 depthStencilAttachmentCount = 0;

			for (UInt32 i = 0; i < MAX_RENDERTARGET_COUNT; ++i)
			{
				if (renderTargets_[i])
				{
					auto* texture = renderTargets_[i]->GetOwnerTexture();
					// 移动端使用TileMemory存储RT内容
					bool useTileMemory = texture->GetDesc().storageMode_ == STORAGE_MODE_MEMORYLESS;
					auto& vkAttachmentDesc = vkAttachmentDescArray[colorAttachmentCount];
					vkAttachmentDesc.flags          = 0;
					vkAttachmentDesc.format         = GfxTextureVulkan::ToVulkanTextureFormat(texture->GetDesc().format_);
					vkAttachmentDesc.samples        = VK_SAMPLE_COUNT_1_BIT;
					vkAttachmentDesc.loadOp         = useTileMemory ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
					vkAttachmentDesc.storeOp        = useTileMemory ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
					vkAttachmentDesc.stencilLoadOp  = useTileMemory ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
					vkAttachmentDesc.stencilStoreOp = useTileMemory ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
					vkAttachmentDesc.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vkAttachmentDesc.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vkColorAttachmentRef[colorAttachmentCount].attachment = colorAttachmentCount;
					vkColorAttachmentRef[colorAttachmentCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vkResolveAttachmentRef[colorAttachmentCount].attachment = colorAttachmentCount;
					vkResolveAttachmentRef[colorAttachmentCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					++colorAttachmentCount;
				}
			}
			if (depthStencil_)
			{
				auto* texture = depthStencil_->GetOwnerTexture();
				// 移动端使用TileMemory存储RT内容
				bool useTileMemory = texture->GetDesc().storageMode_ == STORAGE_MODE_MEMORYLESS;
				auto& vkAttachmentDesc = vkAttachmentDescArray[colorAttachmentCount];
				vkAttachmentDesc.flags          = 0;
				vkAttachmentDesc.format         = GfxTextureVulkan::ToVulkanTextureFormat(texture->GetDesc().format_);
				vkAttachmentDesc.samples        = VK_SAMPLE_COUNT_1_BIT;
				vkAttachmentDesc.loadOp         = useTileMemory ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
				vkAttachmentDesc.storeOp        = useTileMemory ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
				vkAttachmentDesc.stencilLoadOp  = useTileMemory ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
				vkAttachmentDesc.stencilStoreOp = useTileMemory ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
				vkAttachmentDesc.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				vkAttachmentDesc.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				vkDepthStencilAttachmentRef.attachment = colorAttachmentCount;
				vkDepthStencilAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				vkResolveAttachmentRef[colorAttachmentCount].attachment = colorAttachmentCount;
				vkResolveAttachmentRef[colorAttachmentCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				++depthStencilAttachmentCount;
			}
			vkRPCI.attachmentCount = colorAttachmentCount + depthStencilAttachmentCount;
			vkRPCI.pAttachments = vkAttachmentDescArray;

			VkSubpassDescription vkSubpassDesc;
			vkSubpassDesc.flags                   = 0;
			vkSubpassDesc.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
			vkSubpassDesc.inputAttachmentCount    = 0;
			vkSubpassDesc.pInputAttachments       = nullptr;
			vkSubpassDesc.colorAttachmentCount    = colorAttachmentCount;
			vkSubpassDesc.pColorAttachments       = vkColorAttachmentRef;
			vkSubpassDesc.pResolveAttachments     = /*vkResolveAttachmentRef*/nullptr;
			vkSubpassDesc.pDepthStencilAttachment = &vkDepthStencilAttachmentRef;
			vkSubpassDesc.preserveAttachmentCount = 0;
			vkSubpassDesc.pPreserveAttachments    = nullptr;

			vkRPCI.subpassCount    = 1;
			vkRPCI.pSubpasses      = &vkSubpassDesc;
			vkRPCI.dependencyCount = 0;
			vkRPCI.pDependencies   = nullptr;

			VkRenderPass vkRenderPass;
			VULKAN_CHECK(vkCreateRenderPass(vkDevice_, &vkRPCI, &vkAllocCallback_, &vkRenderPass));

			it = vkRenderPassMap_.Insert(MakePair(key, vkRenderPass));
		}

		vkRenderPass_ = it->second_;

		vkRenderPassDirty_ = true;
		renderTargetDirty_ = false;
		depthStencilDirty_ = false;
	}
}

void GfxDeviceVulkan::PrepareShaderStage(GfxShaderVulkan* shader, VkShaderStageFlagBits shaderStage, VkPipelineShaderStageCreateInfo& vkPSSCI)
{
	vkPSSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkPSSCI.pNext = nullptr;
	vkPSSCI.flags = 0;
	vkPSSCI.stage = shaderStage;
	vkPSSCI.module = shader->GetVulkanShader();
	vkPSSCI.pName = "main";
	vkPSSCI.pSpecializationInfo = nullptr;
}

void GfxDeviceVulkan::PrepareVertexInputState(VertexDescription* vertxDesc, VkPipelineVertexInputStateCreateInfo& vkPVISCI)
{
	static VkVertexInputBindingDescription vkVIBD;
	vkVIBD.binding = 0;
	vkVIBD.stride = vertexDesc_->GetStrideSize();
	vkVIBD.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	auto* vertexShaderVulkan = RTTICast<GfxShaderVulkan>(vertexShader_);
	const PODVector<VertexElement>& elements = vertxDesc->GetElements();

	static PODVector<VkVertexInputAttributeDescription> vkVIAD;
	vkVIAD.Clear();

	for (const auto& element : elements)
	{
		const char* semName = VERTEX_ELEMENT_SEM_NAME[element.vertexElementSemantic_];
		if (auto* reflectAttrDesc = vertexShaderVulkan->FindInputDesc(semName, element.index_))
		{
			vkVIAD.Resize(vkVIAD.Size() + 1);
			auto& attrDesc = vkVIAD.Back();
			attrDesc.location = reflectAttrDesc->location_;
			attrDesc.binding = 0;
			attrDesc.format = vulkanElementFormats[element.vertexElementType_];
			attrDesc.offset = element.offset_;
		}
	}

	vkPVISCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vkPVISCI.pNext = nullptr;
	vkPVISCI.flags = 0;
	vkPVISCI.vertexBindingDescriptionCount = 1;
	vkPVISCI.pVertexBindingDescriptions = &vkVIBD;
	vkPVISCI.vertexAttributeDescriptionCount = vkVIAD.Size();
	vkPVISCI.pVertexAttributeDescriptions = &vkVIAD[0];
}

void GfxDeviceVulkan::PrepareInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& vkPIASCI)
{
	vkPIASCI.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vkPIASCI.pNext                  = nullptr;
	vkPIASCI.flags                  = 0;
	vkPIASCI.topology               = vulkanPrimitiveType[primitiveType_];
	vkPIASCI.primitiveRestartEnable = VK_FALSE;
}

void GfxDeviceVulkan::PrepareViewportState(VkPipelineViewportStateCreateInfo& vkPVSCI)
{
	vkPVSCI.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkPVSCI.pNext         = nullptr;
	vkPVSCI.flags         = 0;
	vkPVSCI.viewportCount = 1;
	vkPVSCI.pViewports    = nullptr;
	vkPVSCI.scissorCount  = 1;
	vkPVSCI.pScissors     = nullptr;
}

void GfxDeviceVulkan::PrepareRasterizationState(VkPipelineRasterizationStateCreateInfo& vkPRSCI)
{
	vkPRSCI.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkPRSCI.pNext                   = nullptr;
	vkPRSCI.flags                   = 0;
	vkPRSCI.depthClampEnable        = VK_FALSE;
	vkPRSCI.rasterizerDiscardEnable = VK_FALSE;
	vkPRSCI.polygonMode             = vulkanFillMode[rasterizerState_.fillMode_];
	vkPRSCI.cullMode                = vulkanCullMode[rasterizerState_.cullMode_];
	vkPRSCI.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	if (!Equals(rasterizerState_.depthBias_, 0.0f) && !Equals(rasterizerState_.slopeScaledDepthBias_, 0.0f))
	{
		vkPRSCI.depthBiasEnable         = VK_TRUE;
		vkPRSCI.depthBiasConstantFactor = rasterizerState_.depthBias_;
		vkPRSCI.depthBiasClamp          = F_INFINITY;
		vkPRSCI.depthBiasSlopeFactor    = rasterizerState_.slopeScaledDepthBias_;
		vkPRSCI.lineWidth               = 1;
	}
	else
	{
		vkPRSCI.depthBiasEnable = VK_FALSE;
	}

}

void GfxDeviceVulkan::PrepareMultisampleState(VkPipelineMultisampleStateCreateInfo& vkPMSCI)
{
	vkPMSCI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkPMSCI.pNext                 = nullptr;
	vkPMSCI.flags                 = 0;
	vkPMSCI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	vkPMSCI.sampleShadingEnable   = VK_FALSE;
	vkPMSCI.minSampleShading      = 0.0f; // 保守光栅化，暂时没用，填0
	vkPMSCI.pSampleMask           = nullptr;
	vkPMSCI.alphaToCoverageEnable = VK_FALSE;
	vkPMSCI.alphaToOneEnable      = VK_FALSE;
}

void GfxDeviceVulkan::PrepareDepthStencilState(VkPipelineDepthStencilStateCreateInfo& vkPDSSCI)
{
	VkStencilOpState vkSOS;
	vkSOS.failOp      = vulkanStencilOperation[depthStencilState_.stencilFailOp_];
	vkSOS.passOp      = vulkanStencilOperation[depthStencilState_.depthStencilPassOp_];
	vkSOS.depthFailOp = vulkanStencilOperation[depthStencilState_.depthFailOp_];
	vkSOS.compareOp   = vulkanCompareFunction[depthStencilState_.stencilTestMode_];
	vkSOS.compareMask = depthStencilState_.stencilReadMask_;
	vkSOS.writeMask   = depthStencilState_.stencilWriteMask_;
	vkSOS.reference   = depthStencilState_.stencilRef_;

	vkPDSSCI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	vkPDSSCI.pNext                 = nullptr;
	vkPDSSCI.flags                 = 0;
	vkPDSSCI.depthTestEnable       = depthStencilState_.depthTestMode_ == COMPARISON_ALWAYS ? VK_FALSE : VK_TRUE;
	vkPDSSCI.depthWriteEnable      = depthStencilState_.depthWrite_ ? VK_TRUE : VK_FALSE;
	vkPDSSCI.depthCompareOp        = vulkanCompareFunction[depthStencilState_.depthTestMode_];
	vkPDSSCI.depthBoundsTestEnable = VK_FALSE;
	vkPDSSCI.stencilTestEnable     = depthStencilState_.stencilTest_;
	vkPDSSCI.front                 = vkSOS;
	vkPDSSCI.back                  = vkSOS;
	vkPDSSCI.minDepthBounds        = 0.0f;
	vkPDSSCI.maxDepthBounds        = 1.0f;
}

void GfxDeviceVulkan::PrepareColorBlendState(VkPipelineColorBlendStateCreateInfo& vkPCBSCI, VkPipelineColorBlendAttachmentState& vkPCBAS)
{
	vkPCBSCI.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkPCBSCI.pNext             = nullptr;
	vkPCBSCI.flags             = 0;
	vkPCBSCI.logicOpEnable     = VK_FALSE;
	vkPCBSCI.logicOp           = VK_LOGIC_OP_CLEAR;
	vkPCBSCI.attachmentCount   = 0;
	vkPCBSCI.blendConstants[0] = 0.0f;
	vkPCBSCI.blendConstants[1] = 0.0f;
	vkPCBSCI.blendConstants[2] = 0.0f;
	vkPCBSCI.blendConstants[3] = 0.0f;
	for (UInt32 i = 0; i < MAX_RENDERTARGET_COUNT; ++i)
	{
		if (renderTargets_[i])
			++vkPCBSCI.attachmentCount;
	}
	if (depthStencil_)
		++vkPCBSCI.attachmentCount;
	vkPCBSCI.pAttachments = &vkPCBAS;

	vkPCBAS.blendEnable         = rasterizerState_.blendMode_ == BLEND_REPLACE ? VK_TRUE : VK_FALSE;
	vkPCBAS.srcColorBlendFactor = vulkanSourceRgbBlendFactor[rasterizerState_.blendMode_];
	vkPCBAS.dstColorBlendFactor = vulkanDestRgbBlendFactor[rasterizerState_.blendMode_];
	vkPCBAS.colorBlendOp        = vulkanRgbBlendOpt[rasterizerState_.blendMode_];
	vkPCBAS.srcAlphaBlendFactor = vulkanSourceAlphaBlendFactor[rasterizerState_.blendMode_];
	vkPCBAS.dstAlphaBlendFactor = vulkanDestAlphaBlendFactor[rasterizerState_.blendMode_];
	vkPCBAS.alphaBlendOp        = vulkanAlphaBlendOpt[rasterizerState_.blendMode_];
	vkPCBAS.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

void GfxDeviceVulkan::PrepareDynamicState(VkPipelineDynamicStateCreateInfo& vkPDSCI)
{
	static const VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		VK_DYNAMIC_STATE_STENCIL_REFERENCE,
	};

	vkPDSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	vkPDSCI.pNext = nullptr;
	vkPDSCI.flags = 0;
	vkPDSCI.dynamicStateCount = 4;
	vkPDSCI.pDynamicStates = dynamicStates;
}

VkPipeline GfxDeviceVulkan::CreateRenderPipelineFromCurrentState()
{
	VkPipelineCacheCreateInfo vkPCCI;
	vkPCCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkPCCI.pNext = nullptr;
	vkPCCI.flags = 0;
	vkPCCI.initialDataSize = 0;
	vkPCCI.pInitialData = nullptr;
	VkPipelineCache vkPipelineCache;
	VULKAN_CHECK(vkCreatePipelineCache(vkDevice_, &vkPCCI, &vkAllocCallback_, &vkPipelineCache), nullptr);

	VkPipelineShaderStageCreateInfo vkPSSCI[2];
	PrepareShaderStage(RTTICast<GfxShaderVulkan>(vertexShader_), VK_SHADER_STAGE_VERTEX_BIT, vkPSSCI[0]);
	PrepareShaderStage(RTTICast<GfxShaderVulkan>(pixelShader_), VK_SHADER_STAGE_FRAGMENT_BIT, vkPSSCI[1]);

	VkPipelineVertexInputStateCreateInfo vkPVISCI;
	PrepareVertexInputState(vertexDesc_, vkPVISCI);

	VkPipelineInputAssemblyStateCreateInfo vkPIASCI;
	PrepareInputAssemblyState(vkPIASCI);

	VkPipelineViewportStateCreateInfo vkPVSCI;
	PrepareViewportState(vkPVSCI);

	VkPipelineRasterizationStateCreateInfo vkPRSCI;
	PrepareRasterizationState(vkPRSCI);

	VkPipelineMultisampleStateCreateInfo vkPMSCI;
	PrepareMultisampleState(vkPMSCI);

	VkPipelineDepthStencilStateCreateInfo vkPDSSCI;
	PrepareDepthStencilState(vkPDSSCI);

	VkPipelineColorBlendStateCreateInfo vkPCBSCI;
	VkPipelineColorBlendAttachmentState vkPCBAS;
	PrepareColorBlendState(vkPCBSCI, vkPCBAS);

	// 指定管线哪些操作可以通过vkCmdXXX接口修改
	VkPipelineDynamicStateCreateInfo vkPDSCI;
	PrepareDynamicState(vkPDSCI);

	VkGraphicsPipelineCreateInfo vkGPCI;
	vkGPCI.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkGPCI.pNext               = nullptr;
	vkGPCI.flags               = 0;
	vkGPCI.stageCount          = 2;
	vkGPCI.pStages             = vkPSSCI;
	vkGPCI.pVertexInputState   = &vkPVISCI;
	vkGPCI.pInputAssemblyState = &vkPIASCI;
	vkGPCI.pTessellationState  = nullptr;
	vkGPCI.pViewportState      = &vkPVSCI;
	vkGPCI.pRasterizationState = &vkPRSCI;
	vkGPCI.pMultisampleState   = &vkPMSCI;
	vkGPCI.pColorBlendState    = &vkPCBSCI;
	vkGPCI.pDepthStencilState  = &vkPDSSCI;
	vkGPCI.pDynamicState       = &vkPDSCI;
	vkGPCI.layout              = currentProgram_->GetVulkanPipelineLayout();
	vkGPCI.renderPass          = vkRenderPass_;
	vkGPCI.subpass             = 0;
	vkGPCI.basePipelineHandle  = VK_NULL_HANDLE;
	vkGPCI.basePipelineIndex   = 0;
	VkPipeline vkPipeline;
	VULKAN_CHECK(vkCreateGraphicsPipelines(vkDevice_, vkPipelineCache, 1, &vkGPCI, &vkAllocCallback_, &vkPipeline), nullptr);

	size_t dataSize;
	VULKAN_CHECK(vkGetPipelineCacheData(vkDevice_, vkPipelineCache, &dataSize, nullptr), nullptr);
	if (dataSize > 0)
	{
		PODVector<UInt8> dataBuffer(dataSize);
		VULKAN_CHECK(vkGetPipelineCacheData(vkDevice_, vkPipelineCache, &dataSize, &dataBuffer[0]), nullptr);
		// TODO: write data buffer to local
	}

	VULKAN_CHECK(vkMergePipelineCaches(vkDevice_, vkAllPipelineCache_, 1, &vkPipelineCache), nullptr);

	vkDestroyPipelineCache(vkDevice_, vkPipelineCache, &vkAllocCallback_);

	return vkPipeline;
}

void GfxDeviceVulkan::PrepareRenderPipelineState()
{
	if (vertexDescDirty_ || rasterizerStateDirty_ || depthStencilStateDirty_ || shaderDirty_ || vkRenderPassDirty_)
	{
		UInt32 stateHash = rasterizerState_.GetHash();

		UInt32 depthStateHash = depthStencilState_.GetHash();
		for (UInt32 i = 0; i < 8; ++i)
		{
			stateHash = SDBM_Hash(stateHash, depthStateHash & 0xFF);
			depthStateHash <<= 8;
		}

		UInt64 pointValue = (UInt64)vertexDesc_.Get();
		for (UInt32 i = 0; i < 8; ++i)
		{
			stateHash = SDBM_Hash(stateHash, pointValue & 0xFF);
			pointValue <<= 8;
		}
		pointValue = (UInt64)vertexShader_.Get();
		for (UInt32 i = 0; i < 8; ++i)
		{
			stateHash = SDBM_Hash(stateHash, pointValue & 0xFF);
			pointValue <<= 8;
		}
		pointValue = (UInt64)pixelShader_.Get();
		for (UInt32 i = 0; i < 8; ++i)
		{
			stateHash = SDBM_Hash(stateHash, pointValue & 0xFF);
			pointValue <<= 8;
		}

		if (shaderDirty_)
		{
			auto key = MakePair(WeakPtr<GfxShader>(vertexShader_), WeakPtr<GfxShader>(pixelShader_));
			auto it = programMap_.Find(key);
			if (it == programMap_.End())
			{
				SharedPtr<GfxProgramVulkan> program(new GfxProgramVulkan());
				program->Link(vertexShader_, pixelShader_);
				it = programMap_.Insert(MakePair(key, program));
			}

			currentProgram_ = it->second_;
		}

		auto it = vkPipelineMap_.Find(stateHash);
		if (it == vkPipelineMap_.End())
		{
			VkPipeline newPipeline = CreateRenderPipelineFromCurrentState();
			it = vkPipelineMap_.Insert(MakePair(stateHash, newPipeline));
		}

		vkGraphicsPipeline_ = it->second_;

		vkCmdBindPipeline(vkCmdBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline_);

		vertexDescDirty_ = false;
		rasterizerStateDirty_ = false;
		depthStencilStateDirty_ = false;
		shaderDirty_ = false;
		vkRenderPassDirty_ = false;
	}
}

void GfxDeviceVulkan::PrepareComputePipelineState()
{

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
