#include "GfxShaderResourceView.h"

namespace FlagGG
{

GfxShaderResourceView::GfxShaderResourceView(GfxTexture* ownerTexture, UInt32 viewWidth, UInt32 viewHeight)
	: ownerTexture_(ownerTexture)
	, width_(viewWidth)
	, height_(viewHeight)
{
}

}
