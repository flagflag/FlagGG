//
// OpenGL图形层Buffer
//

#pragma once

#include "GfxDevice/GfxBuffer.h"
#include "GfxDevice/OpenGL/OpenGLInterface.h"

namespace FlagGG
{

class GfxBufferOpenGL : public GfxBuffer
{
	OBJECT_OVERRIDE(GfxBufferOpenGL, GfxBuffer);
public:
	explicit GfxBufferOpenGL();

	~GfxBufferOpenGL() override;

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

protected:
	void ReleaseBuffer();

private:
	GLenum oglTarget_{};
	GLuint oglBuffer_{};
	GLenum oglUsage_{};

	PODVector<UInt8> shadowdData_;
	UInt32 writeOffset_{};
};

}
