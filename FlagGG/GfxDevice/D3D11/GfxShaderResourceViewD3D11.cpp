#include "GfxShaderResourceViewD3D11.h"
#include "GfxD3D11Defines.h"

namespace FlagGG
{

GfxShaderResourceViewD3D11::GfxShaderResourceViewD3D11(GfxTexture* ownerTexture, ID3D11ShaderResourceView* shaderResourceView)
	: GfxShaderResourceView(ownerTexture)
	, shaderResourceView_(shaderResourceView)
{

}

GfxShaderResourceViewD3D11::~GfxShaderResourceViewD3D11()
{
	D3D11_SAFE_RELEASE(shaderResourceView_);
}

}
