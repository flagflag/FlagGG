//
// 软件环境光遮蔽渲染器
//

#pragma once

#include "Graphics/AmbientOcclusionRendering.h"

namespace FlagGG
{

class AmbientOcclusionRenderingSoftware : public AmbientOcclusionRendering
{
public:
	AmbientOcclusionRenderingSoftware();

	~AmbientOcclusionRenderingSoftware() override;

	// 渲染环境关遮蔽
	void RenderAO(const AmbientOcclusionInputData& inputData) override;

	// 获取环境管遮蔽纹理
	Texture2D* GetAmbientOcclusionTexture() const override;

protected:
	void AllocAOTexture(const IntVector2& renderSolution);

private:
	SharedPtr<Texture2D> aoTexture_;
};

}
