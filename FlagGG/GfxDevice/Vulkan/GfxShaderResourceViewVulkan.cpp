#include "GfxShaderResourceViewVulkan.h"
#include "GfxDeviceVulkan.h"

namespace FlagGG
{

GfxShaderResourceViewVulkan::GfxShaderResourceViewVulkan(GfxTexture* ownerTexture, VkImageView imageSamplerView, UInt32 viewWidth, UInt32 viewHeight)
	: GfxShaderResourceView(ownerTexture, viewWidth, viewHeight)
	, imageSamplerView_(imageSamplerView)
{

}

GfxShaderResourceViewVulkan::~GfxShaderResourceViewVulkan()
{
	// Vulkan的imageView既可以做shader sample view也可以做surface view，因此生存周期由onwerTexture控制
	//if (imageSamplerView_)
	//{
	//	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();
	//	vkDestroyImageView(deviceVulkan->GetVulkanDevice(), imageSamplerView_, &deviceVulkan->GetVulkanAllocCallback());
	//	imageSamplerView_ = VK_NULL_HANDLE;
	//}
}

}
