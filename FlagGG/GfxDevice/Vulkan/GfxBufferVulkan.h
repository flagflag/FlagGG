//
// Vulkan图形层Buffer
//

#pragma once

#include "GfxDevice/GfxBuffer.h"
#include "Container/Vector.h"

#include <vulkan-local/vulkan.h>

namespace FlagGG
{

class GfxBufferVulkan : public GfxBuffer
{
	OBJECT_OVERRIDE(GfxBufferVulkan, GfxBuffer);
public:
	explicit GfxBufferVulkan();

	~GfxBufferVulkan() override;

	// 应用当前设置（未调用之前buffer处于不可用状态）
	void Apply(const void* initialDataPtr) override;

	// 更新buffer的数据（usage == BUFFER_DYANMIC时可用）
	void UpdateBuffer(const void* dataPtr) override;

	// 更新buffer一个范围内的数据（usage == BUFFER_DYANMIC时可用）
	void UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size) override;

	// 开始写数据
	void* BeginWrite(UInt32 offset, UInt32 size) override;

	// 结束写数据
	void EndWrite(UInt32 bytesWritten) override;

	// 获取CPU映射数据
	const UInt8* GetShadowData() const override { return nullptr; }

	// 获取vulkan buffer
	VkBuffer& GetVulkanBuffer() { return vkBuffer_; }

protected:
	void* CreateWriteBuffer(UInt32 offset, UInt32 size);

	void SubmitWriteBuffer(UInt32 bytesWritten);

private:
	// vulkan buffer
	VkBuffer vkBuffer_;

	// vulkan buffer关联的显存
	VkDeviceMemory vkMemory_;

	// 当usage为static时，用于临时缓存数据的buffer
	VkBuffer vkWriteBuffer_;

	// 当usage为static时，用于临时缓存数据的memory
	VkDeviceMemory vkWriteMemory_;

	// 上一次调用BeginWrite时，offset的值
	UInt32 lastWriteBeginOffset_;
};

}
