#include "GfxSwapChainVulkan.h"
#include "GfxTextureVulkan.h"
#include "GfxRenderSurfaceVulkan.h"
#include "GfxDeviceVulkan.h"
#include "VulkanDefines.h"
#include "Graphics/Window.h"
#if PLATFORM_WINDOWS
#include <vulkan-local/vulkan_win32.h>
#elif PLATFORM_ANDROID
#include <vulkan-local/vulkan_android.h>
#endif

namespace FlagGG
{

GfxSwapChainVulkan::GfxSwapChainVulkan(Window* window)
	: GfxSwapChain(window)
	, vkSurface_(VK_NULL_HANDLE)
	, vkSwapChain_(VK_NULL_HANDLE)
	, outputWindow_(window)
	, backbufferWidth_(0u)
	, backbufferHeight_(0u)
{

}

GfxSwapChainVulkan::~GfxSwapChainVulkan()
{
	DestroyVKObject();
}

void GfxSwapChainVulkan::DestroyVKObject()
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	if (vkSurface_)
	{
		vkDestroySurfaceKHR(deviceVulkan->GetVulkanInstance(), vkSurface_, &deviceVulkan->GetVulkanAllocCallback());
		vkSurface_ = nullptr;
	}

	if (vkSwapChain_)
	{
		vkDestroySwapchainKHR(deviceVulkan->GetVulkanDevice(), vkSwapChain_, &deviceVulkan->GetVulkanAllocCallback());
		vkSwapChain_ = nullptr;
	}
}

void GfxSwapChainVulkan::Resize(UInt32 width, UInt32 height)
{
	DestroyVKObject();

	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

#if PLATFORM_WINDOWS
	VkWin32SurfaceCreateInfoKHR vkWSCI;
	vkWSCI.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkWSCI.pNext     = nullptr;
	vkWSCI.flags     = 0;
	vkWSCI.hinstance = ::GetModuleHandle(nullptr);
	vkWSCI.hwnd      = (HWND)outputWindow_->GetHandle();
	VULKAN_CHECK(vkCreateWin32SurfaceKHR(deviceVulkan->GetVulkanInstance(), &vkWSCI, &deviceVulkan->GetVulkanAllocCallback(), &vkSurface_));
#elif PLATFORM_ANDROID
	VkAndroidSurfaceCreateInfoKHR vkASCI;
	vkASCI.sType  = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	vkASCI.pNext  = nullptr;
	vkASCI.flags  = 0;
	vkASCI.window = window->GetHandle();
	VULKAN_CHECK(vkCreateAndroidSurfaceKHR(deviceVulkan->GetVulkanInstance(), &vkASCI, &deviceVulkan->GetVulkanAllocCallback(), &vkSurface_));
#else
#error
#endif

	VkSurfaceCapabilitiesKHR vkSurfaceCaps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceVulkan->GetVulkanPhysicalDevice(), vkSurface_, &vkSurfaceCaps);

	backbufferWidth_  = Clamp(width,  vkSurfaceCaps.minImageExtent.width,  vkSurfaceCaps.maxImageExtent.width);
	backbufferHeight_ = Clamp(height, vkSurfaceCaps.minImageExtent.height, vkSurfaceCaps.maxImageExtent.height);

	uint32_t numSurfaceFormats;
	VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(deviceVulkan->GetVulkanPhysicalDevice(), vkSurface_, &numSurfaceFormats, nullptr));
	PODVector<VkSurfaceFormatKHR> surfaceFormats(numSurfaceFormats);
	VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(deviceVulkan->GetVulkanPhysicalDevice(), vkSurface_, &numSurfaceFormats, &surfaceFormats[0]));
	for (UInt32 selectIndex = 0; selectIndex < numSurfaceFormats; ++selectIndex)
	{
		if (surfaceFormats[selectIndex].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			vkSurfaceFormat_ = surfaceFormats[selectIndex];
			break;
		}
	}

	UInt32 compositeAlpha = 0;
	if (vkSurfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
		compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	else if (vkSurfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
		compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	else if (vkSurfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
		compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	else if (vkSurfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	VkSwapchainCreateInfoKHR vkSCI;
	vkSCI.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	vkSCI.pNext                 = nullptr;
	vkSCI.flags                 = 0;
	vkSCI.surface               = vkSurface_;
	vkSCI.minImageCount         = vkSurfaceCaps.minImageCount;
	vkSCI.imageFormat           = vkSurfaceFormat_.format;
	vkSCI.imageColorSpace       = vkSurfaceFormat_.colorSpace;
	vkSCI.imageExtent           = { backbufferWidth_, backbufferHeight_ };
	vkSCI.imageArrayLayers      = 1;
	vkSCI.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	vkSCI.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
	vkSCI.queueFamilyIndexCount = 0;
	vkSCI.pQueueFamilyIndices   = nullptr;
	vkSCI.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vkSCI.compositeAlpha        = (VkCompositeAlphaFlagBitsKHR)compositeAlpha;
	vkSCI.presentMode           = VK_PRESENT_MODE_IMMEDIATE_KHR;
	vkSCI.clipped               = VK_TRUE;
	vkSCI.oldSwapchain          = VK_NULL_HANDLE;
	VULKAN_CHECK(vkCreateSwapchainKHR(deviceVulkan->GetVulkanDevice(), &vkSCI, &deviceVulkan->GetVulkanAllocCallback(), &vkSwapChain_));

	uint32_t numSwapChainImages;
	VULKAN_CHECK(vkGetSwapchainImagesKHR(deviceVulkan->GetVulkanDevice(), vkSwapChain_, &numSwapChainImages, nullptr));
	vkImages_.Resize(numSwapChainImages);
	VULKAN_CHECK(vkGetSwapchainImagesKHR(deviceVulkan->GetVulkanDevice(), vkSwapChain_, &numSwapChainImages, &vkImages_[0]));

	for (UInt32 i = 0; i < numSwapChainImages; ++i)
	{
		auto& colorView = vkColorViews_.Append();

		VkImageViewCreateInfo vkIVCI;
		vkIVCI.sType       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vkIVCI.pNext       = nullptr;
		vkIVCI.flags       = 0;
		vkIVCI.image       = vkImages_[i];
		vkIVCI.viewType    = VK_IMAGE_VIEW_TYPE_2D;
		vkIVCI.format      = vkSurfaceFormat_.format;
		vkIVCI.components  = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, };
		vkIVCI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		vkIVCI.subresourceRange.baseMipLevel   = 0;
		vkIVCI.subresourceRange.levelCount     = 1;
		vkIVCI.subresourceRange.baseArrayLayer = 0;
		vkIVCI.subresourceRange.layerCount     = 1;
		VULKAN_CHECK(vkCreateImageView(deviceVulkan->GetVulkanDevice(), &vkIVCI, &deviceVulkan->GetVulkanAllocCallback(), &colorView));
	}

	renderTarget_ = new GfxRenderSurfaceVulkan(this, vkSurfaceFormat_.format, VK_NULL_HANDLE, backbufferWidth_, backbufferHeight_);

	depthStencilTexture_ = new GfxTextureVulkan();
	depthStencilTexture_->SetFormat(TEXTURE_FORMAT_D24S8);
	depthStencilTexture_->SetWidth(backbufferWidth_);
	depthStencilTexture_->SetHeight(backbufferHeight_);
	depthStencilTexture_->SetDepth(1u);
	depthStencilTexture_->SetLayers(0);
	depthStencilTexture_->SetCube(false);
	depthStencilTexture_->SetMultiSample(1);
	depthStencilTexture_->SetAutoResolve(false);
	depthStencilTexture_->SetUsage(TEXTURE_DEPTHSTENCIL);
	depthStencilTexture_->Apply(nullptr);

	depthStencil_ = RTTICast<GfxRenderSurfaceVulkan>(depthStencilTexture_->GetRenderSurface());

	AcquireNextImage();
}

void GfxSwapChainVulkan::CopyData(GfxTexture* gfxTexture)
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();
	auto* textureVulkan = RTTICast<GfxTextureVulkan>(gfxTexture);

	auto* vkCmdBuffer = deviceVulkan->GetVulkanCmdBuffer();
	bool createNewBuffer = false;
	if (!vkCmdBuffer)
	{
		vkCmdBuffer = deviceVulkan->BeginCommandBuffer();
		createNewBuffer = true;
	}

	VkImageLayout srcImageLayout = textureVulkan->GetVulkanImageLayout();

	textureVulkan->ImageMemoryBarrier(vkCmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	VkImageBlit vkIB;
	vkIB.srcSubresource.aspectMask = textureVulkan->GetVulkanImageAspect();
	vkIB.srcSubresource.mipLevel       = 0;
	vkIB.srcSubresource.baseArrayLayer = 0;
	vkIB.srcSubresource.layerCount     = 1;
	vkIB.srcOffsets[0].x = 0;
	vkIB.srcOffsets[0].y = 0;
	vkIB.srcOffsets[0].z = 0;
	vkIB.srcOffsets[1].x = textureVulkan->GetDesc().width_;
	vkIB.srcOffsets[1].y = textureVulkan->GetDesc().height_;
	vkIB.srcOffsets[1].z = textureVulkan->GetDesc().depth_;
	vkIB.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	vkIB.dstSubresource.mipLevel       = 0;
	vkIB.dstSubresource.baseArrayLayer = 0;
	vkIB.dstSubresource.layerCount     = 1;
	vkIB.dstOffsets[0].x = 0;
	vkIB.dstOffsets[0].y = 0;
	vkIB.dstOffsets[0].z = 0;
	vkIB.dstOffsets[1].x = backbufferWidth_;
	vkIB.dstOffsets[1].y = backbufferHeight_;
	vkIB.dstOffsets[1].z = 1;

	vkCmdBlitImage(vkCmdBuffer, textureVulkan->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImages_[currentImageIdx_], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkIB, VK_FILTER_NEAREST);

	if (srcImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
		textureVulkan->ImageMemoryBarrier(vkCmdBuffer, srcImageLayout);

	if (createNewBuffer)
	{
		deviceVulkan->EndCommandBuffer(vkCmdBuffer, true);
	}
}

void GfxSwapChainVulkan::AcquireNextImage()
{
	VkResult result = vkAcquireNextImageKHR(GetSubsystem<GfxDeviceVulkan>()->GetVulkanDevice(), vkSwapChain_, 0xffffffffffffffffui64, nullptr, VK_NULL_HANDLE, &currentImageIdx_);
	ASSERT(result == VK_SUCCESS);
	if (result == VK_SUCCESS)
	{
		renderTarget_->UpdateImageView(vkColorViews_[currentImageIdx_]);
	}
}

void GfxSwapChainVulkan::Present()
{
	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	deviceVulkan->FlushCommandBuffer();

	VkPresentInfoKHR vkPI;
	vkPI.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	vkPI.pNext              = nullptr;
	vkPI.waitSemaphoreCount = 0;
	vkPI.pWaitSemaphores    = nullptr;
	vkPI.swapchainCount     = 1;
	vkPI.pSwapchains        = &vkSwapChain_;
	vkPI.pImageIndices      = &currentImageIdx_;
	vkPI.pResults           = nullptr;
	VkResult result = vkQueuePresentKHR(deviceVulkan->GetGraphicsQueue(), &vkPI);

	AcquireNextImage();
}

}
