#pragma once

#include <Ultralight/platform/GPUDriver.h>

namespace ultralight
{

class UltralightGPUDriver : public GPUDriver
{
public:
	///
	/// Called before any commands are dispatched during a frame.
	///
	virtual void BeginSynchronize() override;

	///
	/// Called after any commands are dispatched during a frame.
	///
	virtual void EndSynchronize() override;

	///
	/// Get the next available texture ID.
	///
	virtual uint32_t NextTextureId() override;

	///
	/// Create a texture with a certain ID and optional bitmap.
	///
	/// **NOTE**: If the Bitmap is empty (Bitmap::IsEmpty), then a RTT Texture should be created
	/// instead. This will be used as a backing texture for a new RenderBuffer.
	///
	virtual void CreateTexture(uint32_t texture_id, RefPtr<Bitmap> bitmap) override;

	///
	/// Update an existing non-RTT texture with new bitmap data.
	///
	virtual void UpdateTexture(uint32_t texture_id, RefPtr<Bitmap> bitmap) override;

	///
	/// Destroy a texture.
	///
	virtual void DestroyTexture(uint32_t texture_id) override;

	///
	/// Generate the next available render buffer ID.
	///
	virtual uint32_t NextRenderBufferId() override;

	///
	/// Create a render buffer with certain ID and buffer description.
	///
	virtual void CreateRenderBuffer(uint32_t render_buffer_id, const RenderBuffer& buffer) override;

	///
	/// Destroy a render buffer
	///
	virtual void DestroyRenderBuffer(uint32_t render_buffer_id) override;

	///
	/// Generate the next available geometry ID.
	///
	virtual uint32_t NextGeometryId() override;

	///
	/// Create geometry with certain ID and vertex/index data.
	///
	virtual void CreateGeometry(uint32_t geometry_id, const VertexBuffer& vertices,
		const IndexBuffer& indices)
		override;

	///
	/// Update existing geometry with new vertex/index data.
	///
	virtual void UpdateGeometry(uint32_t geometry_id, const VertexBuffer& vertices,
		const IndexBuffer& indices)
		override;

	///
	/// Destroy geometry.
	///
	virtual void DestroyGeometry(uint32_t geometry_id) override;

	///
	/// Update command list (you should copy the commands to your own structure).
	///
	virtual void UpdateCommandList(const CommandList& list) override;
};

}
