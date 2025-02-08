#include "GfxShaderResourceViewVulkan.h"
#include "GfxDeviceVulkan.h"

namespace FlagGG
{

GfxShaderResourceViewVulkan::GfxShaderResourceViewVulkan(GfxTexture* ownerTexture, VkImageView imageSamplerView)
	: GfxShaderResourceView(ownerTexture)
	, imageSamplerView_(imageSamplerView)
{

}

GfxShaderResourceViewVulkan::~GfxShaderResourceViewVulkan()
{
	// Vulkan��imageView�ȿ�����shader sample viewҲ������surface view���������������onwerTexture����
	//if (imageSamplerView_)
	//{
	//	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();
	//	vkDestroyImageView(deviceVulkan->GetVulkanDevice(), imageSamplerView_, &deviceVulkan->GetVulkanAllocCallback());
	//	imageSamplerView_ = VK_NULL_HANDLE;
	//}
}

}
