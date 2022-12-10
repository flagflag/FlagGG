#pragma once

#include "Graphics/GraphicsDef.h"
#include "Graphics/Texture.h"
#include "Resource/Image.h"

#include <string>

namespace FlagGG
{

class FlagGG_API TextureCube : public Texture
{
public:
	TextureCube(Context* context);

	bool SetSize(UInt32 size, UInt32 format, TextureUsage usage = TEXTURE_STATIC, Int32 multiSample = 1);

	bool SetData(CubeMapFace face, UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* data);

	bool SetData(CubeMapFace face, Image* image, bool useAlpha = false);

	bool GetData(CubeMapFace face, UInt32 level, void* dest);

	SharedPtr<Image> GetImage(CubeMapFace face);

	RenderSurface* GetRenderSurface() const override;

	RenderSurface* GetRenderSurface(UInt32 index) const override;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	bool Create() override;

private:
	SharedPtr<RenderSurface> renderSurfaces_[MAX_CUBEMAP_FACES];

	UInt32 faceMemoryUse_[MAX_CUBEMAP_FACES];
};

}
