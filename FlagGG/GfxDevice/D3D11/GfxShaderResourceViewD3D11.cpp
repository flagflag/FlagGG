#include "GfxShaderResourceViewD3D11.h"
#include "GfxD3D11Defines.h"

namespace FlagGG
{

GfxShaderResourceViewD3D11::GfxShaderResourceViewD3D11(GfxTexture* ownerTexture, ID3D11ShaderResourceView* shaderResourceView, UInt32 viewWidth, UInt32 viewHeight)
	: GfxShaderResourceView(ownerTexture, viewWidth, viewHeight)
	, shaderResourceView_(shaderResourceView)
{

}

GfxShaderResourceViewD3D11::~GfxShaderResourceViewD3D11()
{
	D3D11_SAFE_RELEASE(shaderResourceView_);
}

void GfxShaderResourceViewD3D11::SetGpuTag(const String& gpuTag)
{
	if (shaderResourceView_)
	{
		shaderResourceView_->SetPrivateData(WKPDID_D3DDebugObjectName, gpuTag.Length(), gpuTag.CString());
	}
}

}
