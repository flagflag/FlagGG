#include "Ultralight/platform/FontLoader.h"
#include "Ultralight/private/util/RefCountedImpl.h"

#include <Math/Math.h>

namespace ultralight
{

class FontFileImpl : public FontFile, public RefCountedImpl<FontFileImpl>
{
public:
	REF_COUNTED_IMPL(FontFileImpl);

	FontFileImpl(const String& filepath)
		: filepath_(filepath)
	{
		hash_ = FlagGG::HashString(filepath_.utf8().data());
	}

	FontFileImpl(RefPtr<Buffer> buffer)
		: buffer_(buffer)
	{
		hash_ = FlagGG::HashBuffer(buffer->data(), buffer->size());
	}

	///
	/// Whether or not this font file was created from an in-memory buffer.
	///
	virtual bool is_in_memory() const override
	{
		return buffer_;
	}

	///
	/// The file path (if any).
	///
	virtual String filepath() const override
	{
		return filepath_;
	}

	///
	/// The in-memory buffer (if any).
	///
	virtual RefPtr<Buffer> buffer() const override
	{
		return buffer_;
	}

	///
	/// Unique hash (if this is a filepath, only the path string is hashed).
	///
	virtual uint32_t hash() const override
	{
		return hash_;
	}

private:
	String filepath_;

	RefPtr<Buffer> buffer_;

	uint32_t hash_;
};

FontFile::FontFile() = default;

FontFile::~FontFile() = default;

FontFile::FontFile(const FontFile&)
{

}

RefPtr<FontFile> FontFile::Create(const String& filepath)
{
	return RefPtr<FontFile>(new FontFileImpl(filepath));
}

RefPtr<FontFile> FontFile::Create(RefPtr<Buffer> buffer)
{
	return RefPtr<FontFile>(new FontFileImpl(buffer));
}

void FontFile::operator=(const FontFile&)
{

}

FontLoader::~FontLoader() = default;

}
