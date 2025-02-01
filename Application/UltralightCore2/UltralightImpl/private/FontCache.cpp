#include "Ultralight/private/FontCache.h"

namespace ultralight
{

FontCache::FontCache()
{

}

FontCache::~FontCache()
{

}

RefPtr<Font> FontCache::GetFont(void* platform_data, uint64_t font_file_hash, float font_size)
{
	auto it = font_map_.find(font_file_hash);
	if (it != font_map_.end())
		return it->second;

	RefPtr<Font> newFont = Font::Create(platform_data, font_file_hash, font_size);
	font_map_.insert(std::make_pair(font_file_hash, newFont));
	return newFont;
}

uint64_t FontCache::bytes_allocated()
{
	return 0;
}

uint64_t FontCache::bytes_used()
{
	return 0;
}

void FontCache::Recycle()
{

}

void FontCache::PurgeMemory()
{

}

FontCache* FontCache::instance()
{
	static FontCache inst;
	return &inst;
}

}
