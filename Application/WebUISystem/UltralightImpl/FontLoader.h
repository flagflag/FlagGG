#pragma once

#include <Ultralight/platform/FontLoader.h>

namespace ultralight
{

class UltralightFontLoader : public FontLoader
{
public:
	///
	/// Fallback font family name. Will be used if all other fonts fail to load.
	///
	/// @note  This font should be guaranteed to exist (eg, FontLoader::Load won't fail when passed
	///        this font family name).
	///
	virtual String fallback_font() const override;

	///
	/// Fallback font family name that can render the specified characters. Mainly used to support
	/// CJK (Chinese, Japanese, Korean) text display.
	///
	/// @param  characters  One or more UTF-16 characters. This is almost always a single character.
	///
	/// @param  weight      Font weight.
	///
	/// @param  italic      Whether or not italic is requested.
	///
	/// @return  Returns a font family name that can render the text.
	///
	virtual String fallback_font_for_characters(const String& characters, int weight, bool italic) const override;

	///
	/// Get the actual font file data (TTF/OTF) for a given font description.
	///
	/// @param  family  Font family name.
	///
	/// @param  weight  Font weight.
	///
	/// @param  italic  Whether or not italic is requested.
	///
	/// @return  A font file matching the given description (either an on-disk font filepath or an
	///          in-memory file contents). You can return NULL here and the loader will fallback to
	///          another font.
	///
	virtual RefPtr<FontFile> Load(const String& family, int weight, bool italic) override;
};

}
