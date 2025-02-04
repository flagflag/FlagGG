//
// D3D11环境光遮蔽渲染器
//

#pragma once

#include "GfxDevice/AmbientOcclusionRendering.h"

#define GFSDK_SSAO 1

#if GFSDK_SSAO
#include <GFSDK_SSAO.h>
#endif

namespace FlagGG
{

class AmbientOcclusionRenderingD3D11 : public AmbientOcclusionRendering
{
public:
	AmbientOcclusionRenderingD3D11();

	~AmbientOcclusionRenderingD3D11() override;

	// 渲染环境关遮蔽
	void RenderAO(const AmbientOcclusionInputData& inputData) override;

	// 获取环境管遮蔽纹理
	Texture2D* GetAmbientOcclusionTexture() const override;

protected:
	void AllocAOTexture(const IntVector2& renderSolution);

private:
#if GFSDK_SSAO
	GFSDK_SSAO_Parameters aoParameters_;

	GFSDK_SSAO_Context_D3D11* ssaoContext_;

	SharedPtr<Texture2D> aoTexture_;
#endif
};

}
