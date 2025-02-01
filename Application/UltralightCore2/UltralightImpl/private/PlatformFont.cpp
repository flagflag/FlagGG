#include "Ultralight/private/PlatformFont.h"

namespace ultralight
{

static PlatformFontFactory* platformFontFactory = nullptr;

UExport PlatformFontFactory* platform_font_factory()
{
	return platformFontFactory;
}

UExport void set_platform_font_factory(PlatformFontFactory* factory)
{
	platformFontFactory = factory;
}

}
