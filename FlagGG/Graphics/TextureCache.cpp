#include "TextureCache.h"
#include "Graphics/Texture2D.h"

namespace FlagGG
{

TextureCache::TextureCache()
{

}

TextureCache::~TextureCache()
{

}

SharedPtr<Texture2D> TextureCache::GetTexture2D(UInt32 width, UInt32 height, TextureFormat textureFormat, TextureUsage textureUsage, UInt32 numMips)
{
	UInt64 key = UInt64(textureFormat) << 30u | UInt64(numMips) << 26u | width << 14u | height << 2u | UInt64(textureUsage);

	auto& textureArray = textureMap_[key];
	for (auto& it : textureArray)
	{
		if (it->Refs() == 1)
		{
			return it;
		}
	}

	SharedPtr<Texture2D> texture(new Texture2D());
	texture->SetNumLevels(numMips);
	texture->SetSize(width, height, textureFormat, textureUsage);
	textureArray.Push(texture);

	return texture;
}

}
