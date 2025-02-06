//
// 纹理缓存，加速频繁创建、销毁纹理的耗时
//

#pragma once

#include "Core/Subsystem.h"
#include "Core/BaseTypes.h"
#include "Graphics/GraphicsDef.h"
#include "Container/HashMap.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class Texture2D;

class FlagGG_API TextureCache : public Subsystem<TextureCache>
{
public:
	TextureCache();

	~TextureCache();

	SharedPtr<Texture2D> GetTexture2D(UInt32 width, UInt32 height, TextureFormat textureFormat, TextureUsage textureUsage, UInt32 numMips = 1);

private:
	HashMap<UInt64, Vector<SharedPtr<Texture2D>>> textureMap_;
};

}
