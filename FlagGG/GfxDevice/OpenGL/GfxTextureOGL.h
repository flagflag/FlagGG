//
// OpenGL图形层纹理
//

#pragma once

#include "GfxDevice/GfxTexture.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "GfxDevice/OpenGL/OpenGLInterface.h"

namespace FlagGG
{

class GfxRenderSurfaceOpenGL;

class GfxTextureOpenGL : public GfxTexture
{
	OBJECT_OVERRIDE(GfxTextureOpenGL, GfxTexture);
public:
	explicit GfxTextureOpenGL();

	~GfxTextureOpenGL() override;

	// 应用当前设置
	void Apply(const void* initialDataPtr) override;

	// 更新纹理
	void UpdateTexture(const void* dataPtr) override;

	// 更新2D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height) override;

	// 更新3D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth) override;


	// 获取texute2d的render surface
	GfxRenderSurface* GetRenderSurface() const override;

	// 获取render surface
	// 1.TextureArray，index传入array的下标
	// 2.TextureCube，index传入cube的face
	GfxRenderSurface* GetRenderSurface(UInt32 index) const override;


	//
	GLenum GetOGLTarget() const { return oglTarget_; }

	//
	GLuint GetOGLTexture() const { return oglTexture_; }

protected:
	void CreateTexture2D();
	void CreateTexture3D();
	void CreateTextureCube();
	void ReleaseTexture();

private:
	GLuint oglTexture_{};
	GLenum oglTarget_{};
	GLenum oglInternalFormat_{};
	GLenum oglFormat_{};
	GLenum oglType_{};

	// render surfaces
	Vector<SharedPtr<GfxRenderSurfaceOpenGL>> gfxRenderSurfaces_;
};

}
