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
	, vkColorView_(VK_NULL_HANDLE)
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
	vkWSCI.hwnd      = (HWND)outputWindow_->GetWindow();
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
	UInt32 selectIndex = 0;
	for (; selectIndex < numSurfaceFormats; ++selectIndex)
	{
		if (surfaceFormats[selectIndex].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
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
	vkSCI.imageFormat           = surfaceFormats[selectIndex].format;
	vkSCI.imageColorSpace       = surfaceFormats[selectIndex].colorSpace;
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
	PODVector<VkImage> vkImages(numSwapChainImages);
	VULKAN_CHECK(vkGetSwapchainImagesKHR(deviceVulkan->GetVulkanDevice(), vkSwapChain_, &numSwapChainImages, &vkImages[0]));

	VkImageViewCreateInfo vkIVCI;
	vkIVCI.sType       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkIVCI.pNext       = nullptr;
	vkIVCI.flags       = 0;
	vkIVCI.image       = vkImages[0];
	vkIVCI.viewType    = VK_IMAGE_VIEW_TYPE_2D;
	vkIVCI.format      = VK_FORMAT_R8G8B8A8_UNORM;
	vkIVCI.components  = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, };
	vkIVCI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	vkIVCI.subresourceRange.baseMipLevel   = 0;
	vkIVCI.subresourceRange.levelCount     = 1;
	vkIVCI.subresourceRange.baseArrayLayer = 0;
	vkIVCI.subresourceRange.layerCount     = 1;
	VULKAN_CHECK(vkCreateImageView(deviceVulkan->GetVulkanDevice(), &vkIVCI, &deviceVulkan->GetVulkanAllocCallback(), &vkColorView_));

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

	renderTarget_ = new GfxRenderSurfaceVulkan(this, vkColorView_);
	depthStencil_ = RTTICast<GfxRenderSurfaceVulkan>(depthStencilTexture_->GetRenderSurface());
}

void GfxSwapChainVulkan::CopyData(GfxTexture* gfxTexture)
{

}

void GfxSwapChainVulkan::Present()
{
	
}

}
