//
// OpenGL图形层设备
//

#pragma once

#include "GfxDevice/GfxDevice.h"
#include "Utility/Singleton.h"
#include "AsyncFrame/Mutex.h"
#include "Container/HashMap.h"

namespace FlagGG
{

struct GLFrameBufferKey
{
	GLFrameBufferKey(UInt32 idx1 = 0u, UInt32 idx2 = 0u, UInt32 idx3 = 0u, UInt32 idx4 = 0u, UInt32 idx5 = 0u)
		: idx_{ idx1, idx2, idx3, idx4, idx5 }
		, hashValue_(0u)
	{}

	bool operator==(const GLFrameBufferKey& rhs) const
	{
		for (unsigned i = 0; i < MAX_RENDERTARGET_COUNT + 1; ++i)
		{
			if (idx_[i] != rhs.idx_[i])
				return false;
		}
		return true;
	}

	inline UInt32& operator[](unsigned idx)
	{
		return idx_[idx];
	}

	void CalculateHash()
	{
		hashValue_ = 0u;
		UInt32 num = (MAX_RENDERTARGET_COUNT + 1u) * 4u;
		const char* ptr = reinterpret_cast<const char*>(idx_);
		while (num--)
		{
			hashValue_ = *ptr + (hashValue_ << 6u) + (hashValue_ << 16u) - hashValue_;
			++ptr;
		}
	}

	inline UInt32 ToHash() const
	{
		return hashValue_;
	}

	UInt32 hashValue_;
	UInt32 idx_[MAX_RENDERTARGET_COUNT + 1];
};

class GfxDeviceOpenGL : public GfxDevice, public Singleton<GfxDeviceOpenGL, NullMutex>
{
public:
	explicit GfxDeviceOpenGL();

	~GfxDeviceOpenGL() override;

	/**********************************************************/
	/*                        渲染指令                        */
	/**********************************************************/

	// 清理RenderTarget、DepthStencil
	void Clear(ClearTargetFlags flags, const Color& color = Color::TRANSPARENT_BLACK, float depth = 1.0f, unsigned stencil = 0) override;

	// 提交渲染指令
	void Draw(UInt32 vertexStart, UInt32 vertexCount) override;

	// 提交渲染指令
	void DrawIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart) override;

	// 提交渲染指令
	void DrawIndexedInstanced(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 instanceCount) override;

	// Flush
	void Flush() override;


	/**********************************************************/
	/*                     创建Gfx对象                        */
	/**********************************************************/

	// 创建交换链
	GfxSwapChain* CreateSwapChain(Window* window) override;

	// 创建纹理
	GfxTexture* CreateTexture() override;

	// 创建buffer
	GfxBuffer* CreateBuffer() override;

	// 创建shader
	GfxShader* CreateShader() override;

	// 创建gpu program
	GfxProgram* CreateProgram() override;

protected:
	void SetFrameBuffer();

	void PrepareDraw();

private:
	HashMap<GLFrameBufferKey, GLuint> frameBufferMap_;
};

}
