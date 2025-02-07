//
// ����������ڱ���Ⱦ��
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

	// ��Ⱦ�������ڱ�
	void RenderAO(const AmbientOcclusionInputData& inputData) override;

	// ��ȡ�������ڱ�����
	Texture2D* GetAmbientOcclusionTexture() const override;

protected:
	void AllocAOTexture(const IntVector2& renderSolution);

private:
	SharedPtr<Texture2D> aoTexture_;
};

}
