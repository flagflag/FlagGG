#include "GfxTextureVulkan.h"
#include "GfxDeviceVulkan.h"
#include "GfxRenderSurfaceVulkan.h"
#include "VulkanDefines.h"
#include "Log.h"
#include "Memory/Memory.h"

namespace FlagGG
{

struct VulkanTextureFormatInfo
{
	VkFormat fmt_;
	VkFormat fmtSrv_;
	VkFormat fmtDsv_;
	VkFormat fmtSrgb_;
	VkComponentMapping components_;
};

static const VulkanTextureFormatInfo vulkanTextureFormat[] =
{
	{ VK_FORMAT_BC1_RGB_UNORM_BLOCK,       VK_FORMAT_BC1_RGB_UNORM_BLOCK,      VK_FORMAT_UNDEFINED,           VK_FORMAT_BC1_RGB_SRGB_BLOCK,       { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC1
	{ VK_FORMAT_BC2_UNORM_BLOCK,           VK_FORMAT_BC2_UNORM_BLOCK,          VK_FORMAT_UNDEFINED,           VK_FORMAT_BC2_SRGB_BLOCK,           { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC2
	{ VK_FORMAT_BC3_UNORM_BLOCK,           VK_FORMAT_BC3_UNORM_BLOCK,          VK_FORMAT_UNDEFINED,           VK_FORMAT_BC3_SRGB_BLOCK,           { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC3
	{ VK_FORMAT_BC4_UNORM_BLOCK,           VK_FORMAT_BC4_UNORM_BLOCK,          VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC4
	{ VK_FORMAT_BC5_UNORM_BLOCK,           VK_FORMAT_BC5_UNORM_BLOCK,          VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC5
	{ VK_FORMAT_BC6H_SFLOAT_BLOCK,         VK_FORMAT_BC6H_SFLOAT_BLOCK,        VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC6H
	{ VK_FORMAT_BC7_UNORM_BLOCK,           VK_FORMAT_BC7_UNORM_BLOCK,          VK_FORMAT_UNDEFINED,           VK_FORMAT_BC7_SRGB_BLOCK,           { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BC7
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ETC1
	{ VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,   VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,   { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ETC2
	{ VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ETC2A
	{ VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ETC2A1
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // PTC12
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // PTC14
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // PTC12A
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // PTC14A
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // PTC22
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // PTC24
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ATC
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ATCE
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ATCI
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ASTC4x4
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ASTC5x5
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ASTC6x6
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ASTC8x5
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ASTC8x6
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // ASTC10x5
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // Unknown
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R1
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // A8
	{ VK_FORMAT_R8_UNORM,                  VK_FORMAT_R8_UNORM,                 VK_FORMAT_UNDEFINED,           VK_FORMAT_R8_SRGB,                  { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R8
	{ VK_FORMAT_R8_SINT,                   VK_FORMAT_R8_SINT,                  VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R8I
	{ VK_FORMAT_R8_UINT,                   VK_FORMAT_R8_UINT,                  VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R8U
	{ VK_FORMAT_R8_SNORM,                  VK_FORMAT_R8_SNORM,                 VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R8S
	{ VK_FORMAT_R16_UNORM,                 VK_FORMAT_R16_UNORM,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R16
	{ VK_FORMAT_R16_SINT,                  VK_FORMAT_R16_SINT,                 VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R16I
	{ VK_FORMAT_R16_UNORM,                 VK_FORMAT_R16_UNORM,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R16U
	{ VK_FORMAT_R16_SFLOAT,                VK_FORMAT_R16_SFLOAT,               VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R16F
	{ VK_FORMAT_R16_SNORM,                 VK_FORMAT_R16_SNORM,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R16S
	{ VK_FORMAT_R32_SINT,                  VK_FORMAT_R32_SINT,                 VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R32I
	{ VK_FORMAT_R32_UINT,                  VK_FORMAT_R32_UINT,                 VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R32U
	{ VK_FORMAT_R32_SFLOAT,                VK_FORMAT_R32_SFLOAT,               VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R32F
	{ VK_FORMAT_R8G8_UNORM,                VK_FORMAT_R8G8_UNORM,               VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8_SRGB,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG8
	{ VK_FORMAT_R8G8_SINT,                 VK_FORMAT_R8G8_SINT,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG8I
	{ VK_FORMAT_R8G8_UINT,                 VK_FORMAT_R8G8_UINT,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG8U
	{ VK_FORMAT_R8G8_SNORM,                VK_FORMAT_R8G8_SNORM,               VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG8S
	{ VK_FORMAT_R16G16_UNORM,              VK_FORMAT_R16G16_UNORM,             VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG16
	{ VK_FORMAT_R16G16_SINT,               VK_FORMAT_R16G16_SINT,              VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG16I
	{ VK_FORMAT_R16G16_UINT,               VK_FORMAT_R16G16_UINT,              VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG16U
	{ VK_FORMAT_R16G16_SFLOAT,             VK_FORMAT_R16G16_SFLOAT,            VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG16F
	{ VK_FORMAT_R16G16_SNORM,              VK_FORMAT_R16G16_SNORM,             VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG16S
	{ VK_FORMAT_R32G32_SINT,               VK_FORMAT_R32G32_SINT,              VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG32I
	{ VK_FORMAT_R32G32_UINT,               VK_FORMAT_R32G32_UINT,              VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG32U
	{ VK_FORMAT_R32G32_SFLOAT,             VK_FORMAT_R32G32_SFLOAT,            VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG32F
	{ VK_FORMAT_R8G8B8_UNORM,              VK_FORMAT_R8G8B8_UNORM,             VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8B8_SRGB,              { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB8
	{ VK_FORMAT_R8G8B8_SINT,               VK_FORMAT_R8G8B8_SINT,              VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8B8_SRGB,              { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB8I
	{ VK_FORMAT_R8G8B8_UINT,               VK_FORMAT_R8G8B8_UINT,              VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8B8_SRGB,              { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB8U
	{ VK_FORMAT_R8G8B8_SNORM,              VK_FORMAT_R8G8B8_SNORM,             VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB8S
	{ VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,   VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB9E5F
	{ VK_FORMAT_B8G8R8A8_UNORM,            VK_FORMAT_B8G8R8A8_UNORM,           VK_FORMAT_UNDEFINED,           VK_FORMAT_B8G8R8A8_SRGB,            { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // BGRA8
	{ VK_FORMAT_R8G8B8A8_UNORM,            VK_FORMAT_R8G8B8A8_UNORM,           VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8B8A8_SRGB,            { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA8
	{ VK_FORMAT_R8G8B8A8_SINT,             VK_FORMAT_R8G8B8A8_SINT,            VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8B8A8_SRGB,            { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA8I
	{ VK_FORMAT_R8G8B8A8_UINT,             VK_FORMAT_R8G8B8A8_UINT,            VK_FORMAT_UNDEFINED,           VK_FORMAT_R8G8B8A8_SRGB,            { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA8U
	{ VK_FORMAT_R8G8B8A8_SNORM,            VK_FORMAT_R8G8B8A8_SNORM,           VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA8S
	{ VK_FORMAT_R16G16B16A16_UNORM,        VK_FORMAT_R16G16B16A16_UNORM,       VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA16
	{ VK_FORMAT_R16G16B16A16_SINT,         VK_FORMAT_R16G16B16A16_SINT,        VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA16I
	{ VK_FORMAT_R16G16B16A16_UINT,         VK_FORMAT_R16G16B16A16_UINT,        VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA16U
	{ VK_FORMAT_R16G16B16A16_SFLOAT,       VK_FORMAT_R16G16B16A16_SFLOAT,      VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA16F
	{ VK_FORMAT_R16G16B16A16_SNORM,        VK_FORMAT_R16G16B16A16_SNORM,       VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA16S
	{ VK_FORMAT_R32G32B32A32_SINT,         VK_FORMAT_R32G32B32A32_SINT,        VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA32I
	{ VK_FORMAT_R32G32B32A32_UINT,         VK_FORMAT_R32G32B32A32_UINT,        VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA32U
	{ VK_FORMAT_R32G32B32A32_SFLOAT,       VK_FORMAT_R32G32B32A32_SFLOAT,      VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGBA32F
	{ VK_FORMAT_B5G6R5_UNORM_PACK16,       VK_FORMAT_B5G6R5_UNORM_PACK16,      VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // R5G6B5
	{ VK_FORMAT_B4G4R4A4_UNORM_PACK16,     VK_FORMAT_B4G4R4A4_UNORM_PACK16,    VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_G,        VK_COMPONENT_SWIZZLE_R,        VK_COMPONENT_SWIZZLE_A,        VK_COMPONENT_SWIZZLE_B        } }, // RGBA4
	{ VK_FORMAT_A1R5G5B5_UNORM_PACK16,     VK_FORMAT_A1R5G5B5_UNORM_PACK16,    VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB5A1
	{ VK_FORMAT_A2R10G10B10_UNORM_PACK32,  VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RGB10A2
	{ VK_FORMAT_B10G11R11_UFLOAT_PACK32,   VK_FORMAT_B10G11R11_UFLOAT_PACK32,  VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // RG11B10F
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_UNDEFINED,                VK_FORMAT_UNDEFINED,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // UnknownDepth
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_R16_UNORM,                VK_FORMAT_D16_UNORM,           VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D16
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_X8_D24_UNORM_PACK32,      VK_FORMAT_D24_UNORM_S8_UINT,   VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D24
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_X8_D24_UNORM_PACK32,      VK_FORMAT_D24_UNORM_S8_UINT,   VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D24S8
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_X8_D24_UNORM_PACK32,      VK_FORMAT_D24_UNORM_S8_UINT,   VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D32
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_R32_SFLOAT,               VK_FORMAT_D32_SFLOAT,          VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D16F
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_R32_SFLOAT,               VK_FORMAT_D32_SFLOAT,          VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D24F
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_R32_SFLOAT,               VK_FORMAT_D32_SFLOAT,          VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D32F
	{ VK_FORMAT_UNDEFINED,                 VK_FORMAT_X8_D24_UNORM_PACK32,      VK_FORMAT_D24_UNORM_S8_UINT,   VK_FORMAT_UNDEFINED,                { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY } }, // D0S8
};

GfxTextureVulkan::GfxTextureVulkan()
	: vkFormat_(VK_FORMAT_UNDEFINED)
	, vkImageAspectMask_(0)
	, vkImageLayout_(VK_IMAGE_LAYOUT_UNDEFINED)
	, vkImage_(VK_NULL_HANDLE)
	, vkMemory_(VK_NULL_HANDLE)
	, vkImageView_(VK_NULL_HANDLE)
	, vkDepthView_(VK_NULL_HANDLE)
	, vkStorageView_(VK_NULL_HANDLE)
{

}

GfxTextureVulkan::~GfxTextureVulkan()
{

}

void GfxTextureVulkan::Apply(const void* initialDataPtr)
{
	if (!textureDesc_.width_ || !textureDesc_.height_)
	{
		FLAGGG_LOG_ERROR("Texture's width or height is invalid.");
		return;
	}

	textureDesc_.levels_ = GfxTextureUtils::CheckMaxLevels(textureDesc_.width_, textureDesc_.height_, textureDesc_.requestedLevels_);

	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

// 创建vulkan image
	if (textureDesc_.format_ > TEXTURE_FORMAT_UNKNOWN_DEPTH)
		vkFormat_ = vulkanTextureFormat[textureDesc_.format_].fmtDsv_;
	else
		vkFormat_ = textureDesc_.sRGB_ ? vulkanTextureFormat[textureDesc_.format_].fmtSrgb_ : vulkanTextureFormat[textureDesc_.format_].fmt_;
	vkImageLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageType vkImageType = VK_IMAGE_TYPE_1D;
	VkImageViewType vkImageViewType = VK_IMAGE_VIEW_TYPE_1D;
	if (textureDesc_.depth_ == 1)
	{
		vkImageType = VK_IMAGE_TYPE_2D;
		vkImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	}
	else
	{
		vkImageType = VK_IMAGE_TYPE_3D;
		vkImageViewType = VK_IMAGE_VIEW_TYPE_3D;
	}
	VkImageCreateInfo vkICI;
	vkICI.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vkICI.pNext                 = nullptr;
	vkICI.flags                 = textureDesc_.isCube_ ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	vkICI.imageType             = vkImageType;
	vkICI.format                = vkFormat_;
	vkICI.extent.width          = textureDesc_.width_;
	vkICI.extent.height         = textureDesc_.height_;
	vkICI.extent.depth          = textureDesc_.depth_;
	vkICI.mipLevels             = textureDesc_.levels_;
	vkICI.arrayLayers           = Max(textureDesc_.layers_, 1u);
	vkICI.samples               = VK_SAMPLE_COUNT_1_BIT;
	vkICI.tiling                = VK_IMAGE_TILING_OPTIMAL;
	vkICI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
		vkICI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
		vkICI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (textureDesc_.bindFlags_ & TEXTURE_BIND_COMPUTE_WRITE)
		vkICI.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
	vkICI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	vkICI.queueFamilyIndexCount = 0;
	vkICI.pQueueFamilyIndices   = nullptr;
	vkICI.initialLayout         = vkImageLayout_;

	VULKAN_CHECK(vkCreateImage(deviceVulkan->GetVulkanDevice(), &vkICI, &deviceVulkan->GetVulkanAllocCallback(), &vkImage_));

// 分配显存
	VkMemoryRequirements vkMR;
	vkGetImageMemoryRequirements(deviceVulkan->GetVulkanDevice(), vkImage_, &vkMR);

	VkMemoryAllocateInfo vkMAI;
	vkMAI.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMAI.pNext           = nullptr;
	vkMAI.allocationSize  = vkMR.size;
	vkMAI.memoryTypeIndex = deviceVulkan->GetVulkanMemoryTypeIndex(vkMR.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VULKAN_CHECK(vkAllocateMemory(deviceVulkan->GetVulkanDevice(), &vkMAI, &deviceVulkan->GetVulkanAllocCallback(), &vkMemory_));

// 关联显存
	VULKAN_CHECK(vkBindImageMemory(deviceVulkan->GetVulkanDevice(), vkImage_, vkMemory_, 0));

	if (initialDataPtr)
		UpdateTexture(initialDataPtr);
	else
	{
		VkCommandBuffer vkCmdBuffer = deviceVulkan->BeginCommandBuffer();
		ImageMemoryBarrier(vkCmdBuffer, (textureDesc_.bindFlags_ & TEXTURE_BIND_COMPUTE_WRITE) ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		deviceVulkan->EndCommandBuffer(vkCmdBuffer, true);
	}

// 创建TextureView
	{
		if (textureDesc_.format_ > TEXTURE_FORMAT_UNKNOWN_DEPTH)
		{
			vkImageAspectMask_ = 0;
			if (textureDesc_.format_ == TEXTURE_FORMAT_D16 ||
				textureDesc_.format_ == TEXTURE_FORMAT_D24 ||
				textureDesc_.format_ == TEXTURE_FORMAT_D24S8 ||
				textureDesc_.format_ == TEXTURE_FORMAT_D32 ||
				textureDesc_.format_ == TEXTURE_FORMAT_D16F ||
				textureDesc_.format_ == TEXTURE_FORMAT_D24F ||
				textureDesc_.format_ == TEXTURE_FORMAT_D32F)
			{
				vkImageAspectMask_ |= VK_IMAGE_ASPECT_DEPTH_BIT;
			}
			if (textureDesc_.format_ == TEXTURE_FORMAT_D24S8 ||
				textureDesc_.format_ == TEXTURE_FORMAT_D0S8)
			{
				vkImageAspectMask_ |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
			vkImageAspectMask_ = VK_IMAGE_ASPECT_COLOR_BIT;

		VkImageViewCreateInfo vkIVCI;
		vkIVCI.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vkIVCI.pNext                           = nullptr;
		vkIVCI.flags                           = 0;
		vkIVCI.image                           = vkImage_;
		vkIVCI.viewType                        = vkImageViewType;
		vkIVCI.format                          = vkICI.format;
		vkIVCI.components                      = vulkanTextureFormat[textureDesc_.format_].components_;
		vkIVCI.subresourceRange.aspectMask     = vkImageAspectMask_;
		vkIVCI.subresourceRange.baseMipLevel   = 0;
		vkIVCI.subresourceRange.levelCount     = vkICI.mipLevels;
		vkIVCI.subresourceRange.baseArrayLayer = 0;
		vkIVCI.subresourceRange.layerCount     = vkICI.arrayLayers;
		vkCreateImageView(deviceVulkan->GetVulkanDevice(), &vkIVCI, &deviceVulkan->GetVulkanAllocCallback(), &vkImageView_);
	}

// 创建DepthView
	if ((vkImageAspectMask_ & VK_IMAGE_ASPECT_DEPTH_BIT) && (vkImageAspectMask_ & VK_IMAGE_ASPECT_STENCIL_BIT))
	{
		VkImageViewCreateInfo vkIVCI;
		vkIVCI.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vkIVCI.pNext                           = nullptr;
		vkIVCI.flags                           = 0;
		vkIVCI.image                           = vkImage_;
		vkIVCI.viewType                        = textureDesc_.isCube_ ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : vkImageViewType;
		vkIVCI.format                          = vkICI.format;
		vkIVCI.components                      = vulkanTextureFormat[textureDesc_.format_].components_;
		vkIVCI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
		vkIVCI.subresourceRange.baseMipLevel   = 0;
		vkIVCI.subresourceRange.levelCount     = vkICI.mipLevels;
		vkIVCI.subresourceRange.baseArrayLayer = 0;
		vkIVCI.subresourceRange.layerCount     = vkICI.arrayLayers;
		vkCreateImageView(deviceVulkan->GetVulkanDevice(), &vkIVCI, &deviceVulkan->GetVulkanAllocCallback(), &vkDepthView_);
	}

// 创建StorageView
	if (textureDesc_.bindFlags_ & TEXTURE_BIND_COMPUTE_WRITE)
	{
		VkImageViewCreateInfo vkIVCI;
		vkIVCI.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vkIVCI.pNext                           = nullptr;
		vkIVCI.flags                           = 0;
		vkIVCI.image                           = vkImage_;
		vkIVCI.viewType                        = textureDesc_.isCube_ ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : vkImageViewType;
		vkIVCI.format                          = vkICI.format;
		vkIVCI.components                      = vulkanTextureFormat[textureDesc_.format_].components_;
		vkIVCI.subresourceRange.aspectMask     = vkImageAspectMask_;
		vkIVCI.subresourceRange.baseMipLevel   = 0;
		vkIVCI.subresourceRange.levelCount     = vkICI.mipLevels;
		vkIVCI.subresourceRange.baseArrayLayer = 0;
		vkIVCI.subresourceRange.layerCount     = vkICI.arrayLayers;
		vkCreateImageView(deviceVulkan->GetVulkanDevice(), &vkIVCI, &deviceVulkan->GetVulkanAllocCallback(), &vkStorageView_);
	}

	gfxRenderSurfaces_.Clear();
	gfxRenderSurfaces_.Push(MakeShared<GfxRenderSurfaceVulkan>(this));
}

VkImageView GfxTextureVulkan::GetVulkanSamplerView()
{
	return vkDepthView_ ? vkDepthView_ : vkImageView_;
}

void GfxTextureVulkan::UpdateTexture(const void* dataPtr)
{

}

void GfxTextureVulkan::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{
	if (!vkImage_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid or format conflict.");
		return;
	}

	if (!dataPtr)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> set nullptr data.");
		return;
	}

	if (level >= textureDesc_.levels_)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal mip level.");
		return;
	}

	const TextureMipInfo mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || width <= 0 || height <= 0)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal dimensions.");
		return;
	}

	auto* deviceVulkan = GetSubsystem<GfxDeviceVulkan>();

	UInt32 subResourceRowSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, width);
	UInt32 subResourceSize = subResourceRowSize * height;

	VkBuffer vkTempBuffer;
	VkDeviceMemory vkTempMemory;

// 创建临时buffer
	VkBufferCreateInfo vkBCI;
	vkBCI.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBCI.pNext                 = nullptr;
	vkBCI.flags                 = 0;
	vkBCI.size                  = subResourceSize;
	vkBCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	vkBCI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	vkBCI.queueFamilyIndexCount = 0;
	vkBCI.pQueueFamilyIndices   = nullptr;

	VULKAN_CHECK(vkCreateBuffer(deviceVulkan->GetVulkanDevice(), &vkBCI, &deviceVulkan->GetVulkanAllocCallback(), &vkTempBuffer));

// 分配CPU内存 => VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	VkMemoryRequirements vkMR;
	vkGetBufferMemoryRequirements(deviceVulkan->GetVulkanDevice(), vkTempBuffer, &vkMR);

	VkMemoryAllocateInfo vkMAI;
	vkMAI.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMAI.pNext           = nullptr;
	vkMAI.allocationSize  = vkMR.size;
	vkMAI.memoryTypeIndex = deviceVulkan->GetVulkanMemoryTypeIndex(vkMR.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VULKAN_CHECK(vkAllocateMemory(deviceVulkan->GetVulkanDevice(), &vkMAI, &deviceVulkan->GetVulkanAllocCallback(), &vkTempMemory));

// 关联CPU内存
	VULKAN_CHECK(vkBindBufferMemory(deviceVulkan->GetVulkanDevice(), vkTempBuffer, vkTempMemory, 0));

// 创建临时buffer，拷贝到vkImage_
	void* cpuMemory = nullptr;
	VULKAN_CHECK(vkMapMemory(deviceVulkan->GetVulkanDevice(), vkTempMemory, 0, subResourceSize, 0, &cpuMemory));
	Memory::Memcpy(cpuMemory, dataPtr, subResourceSize);
	vkUnmapMemory(deviceVulkan->GetVulkanDevice(), vkTempMemory);

	VkCommandBuffer vkCmdBuffer = deviceVulkan->BeginCommandBuffer();

	VkBufferImageCopy vkBufferCopyInfo;
	vkBufferCopyInfo.bufferOffset                    = 0;
	vkBufferCopyInfo.bufferRowLength                 = 0;
	vkBufferCopyInfo.bufferImageHeight               = 0;
	vkBufferCopyInfo.imageSubresource.aspectMask     = vkImageAspectMask_;
	vkBufferCopyInfo.imageSubresource.mipLevel       = level;
	vkBufferCopyInfo.imageSubresource.baseArrayLayer = index;
	vkBufferCopyInfo.imageSubresource.layerCount     = 1;
	vkBufferCopyInfo.imageOffset                     = { Int32(x), Int32(y), 0 };
	vkBufferCopyInfo.imageExtent                     = { width, height, 1 };
	ImageMemoryBarrier(vkCmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdCopyBufferToImage(vkCmdBuffer, vkTempBuffer, vkImage_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferCopyInfo);
	ImageMemoryBarrier(vkCmdBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	deviceVulkan->EndCommandBuffer(vkCmdBuffer, true);

	vkFreeMemory(deviceVulkan->GetVulkanDevice(), vkTempMemory, &deviceVulkan->GetVulkanAllocCallback());
	vkDestroyBuffer(deviceVulkan->GetVulkanDevice(), vkTempBuffer, &deviceVulkan->GetVulkanAllocCallback());
}

void GfxTextureVulkan::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{

}

void GfxTextureVulkan::UpdateTexture(GfxTexture* gfxTexture)
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
	VkImageLayout dstImageLayout = GetVulkanImageLayout();

	textureVulkan->ImageMemoryBarrier(vkCmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	ImageMemoryBarrier(vkCmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

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
	vkIB.dstSubresource.aspectMask     = GetVulkanImageAspect();
	vkIB.dstSubresource.mipLevel       = 0;
	vkIB.dstSubresource.baseArrayLayer = 0;
	vkIB.dstSubresource.layerCount     = 1;
	vkIB.dstOffsets[0].x = 0;
	vkIB.dstOffsets[0].y = 0;
	vkIB.dstOffsets[0].z = 0;
	vkIB.dstOffsets[1].x = GetDesc().width_;
	vkIB.dstOffsets[1].y = GetDesc().height_;
	vkIB.dstOffsets[1].z = 1;

	vkCmdBlitImage(vkCmdBuffer, textureVulkan->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkIB, VK_FILTER_NEAREST);

	if (srcImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
		textureVulkan->ImageMemoryBarrier(vkCmdBuffer, srcImageLayout);
	if (dstImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
		ImageMemoryBarrier(vkCmdBuffer, dstImageLayout);

	if (createNewBuffer)
	{
		deviceVulkan->EndCommandBuffer(vkCmdBuffer, true);
	}
}

GfxRenderSurface* GfxTextureVulkan::GetRenderSurface() const
{
	return gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[0] : nullptr;
}

GfxRenderSurface* GfxTextureVulkan::GetRenderSurface(UInt32 index) const
{
	return index < gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[index] : nullptr;
}

void GfxTextureVulkan::SetImageMemoryBarrier(VkCommandBuffer _commandBuffer, VkImage _image, VkImageAspectFlags _aspectMask, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _levelCount, uint32_t _layerCount)
{
	ASSERT(true
		&& _newLayout != VK_IMAGE_LAYOUT_UNDEFINED
		&& _newLayout != VK_IMAGE_LAYOUT_PREINITIALIZED
		, "_newLayout cannot use VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED."
	);

	VkAccessFlags srcAccessMask = 0;
	VkAccessFlags dstAccessMask = 0;

	switch (_oldLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		//			srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_GENERAL:
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		srcAccessMask |= VK_ACCESS_SHADER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		srcAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
		break;

	default:
		break;
	}

	switch (_newLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		break;

	case VK_IMAGE_LAYOUT_GENERAL:
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		// aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		dstAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
		break;

	default:
		break;
	}

	VkImageMemoryBarrier imb;
	imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imb.pNext = NULL;
	imb.srcAccessMask = srcAccessMask;
	imb.dstAccessMask = dstAccessMask;
	imb.oldLayout = _oldLayout;
	imb.newLayout = _newLayout;
	imb.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imb.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imb.image = _image;
	imb.subresourceRange.aspectMask = _aspectMask;
	imb.subresourceRange.baseMipLevel = 0;
	imb.subresourceRange.levelCount = _levelCount;
	imb.subresourceRange.baseArrayLayer = 0;
	imb.subresourceRange.layerCount = _layerCount;
	vkCmdPipelineBarrier(_commandBuffer
		, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		, 0
		, 0
		, NULL
		, 0
		, NULL
		, 1
		, &imb
	);
}


void GfxTextureVulkan::ImageMemoryBarrier(VkCommandBuffer vkCmdBuffer, VkImageLayout vkImageLayout)
{
	//if (vkImageLayout_ == vkImageLayout)
	//	return;

	//SetImageMemoryBarrier(vkCmdBuffer, vkImage_, vkImageAspectMask_, vkImageLayout_, vkImageLayout, GetDesc().levels_, Max(GetDesc().layers_, 1u));

	//vkImageLayout_ = vkImageLayout;
}

}
