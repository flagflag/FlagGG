//
// D3D11图形层TextureView
//

#pragma once

#include "GfxDevice/GfxShaderResourceView.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxShaderResourceViewD3D11 : public GfxShaderResourceView
{
	OBJECT_OVERRIDE(GfxShaderResourceViewD3D11, GfxShaderResourceView);
public:
	GfxShaderResourceViewD3D11(GfxTexture* ownerTexture, ID3D11ShaderResourceView* shaderResourceView);

	~GfxShaderResourceViewD3D11() override;

	// 获取ID3D11ShaderResourceView*
	ID3D11ShaderResourceView* GetD3D11ShaderResourceView() const { return shaderResourceView_; }

private:
	// shader resource view
	ID3D11ShaderResourceView* shaderResourceView_{};
};

}
