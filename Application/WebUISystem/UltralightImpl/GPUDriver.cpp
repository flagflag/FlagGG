#include "GPUDriver.h"

namespace ultralight
{

void UltralightGPUDriver::BeginSynchronize()
{
}

void UltralightGPUDriver::EndSynchronize()
{
}

uint32_t UltralightGPUDriver::NextTextureId()
{
	return 0;
}

void UltralightGPUDriver::CreateTexture(uint32_t texture_id, RefPtr<Bitmap> bitmap)
{
}

void UltralightGPUDriver::UpdateTexture(uint32_t texture_id, RefPtr<Bitmap> bitmap)
{
}

void UltralightGPUDriver::DestroyTexture(uint32_t texture_id)
{
}

uint32_t UltralightGPUDriver::NextRenderBufferId()
{
	return 0;
}

void UltralightGPUDriver::CreateRenderBuffer(uint32_t render_buffer_id, const RenderBuffer& buffer)
{
}

void UltralightGPUDriver::DestroyRenderBuffer(uint32_t render_buffer_id)
{
}

uint32_t UltralightGPUDriver::NextGeometryId()
{
	return 0;
}

void UltralightGPUDriver::CreateGeometry(uint32_t geometry_id, const VertexBuffer& vertices,
	const IndexBuffer& indices)
{

}

void UltralightGPUDriver::UpdateGeometry(uint32_t geometry_id, const VertexBuffer& vertices,
	const IndexBuffer& indices)
{

}

void UltralightGPUDriver::DestroyGeometry(uint32_t geometry_id)
{
}

void UltralightGPUDriver::UpdateCommandList(const CommandList& list)
{
}

}

