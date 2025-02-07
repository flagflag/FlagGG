#include "AmbientOcclusionRenderingSoftware.h"
#include "Graphics/Texture2D.h"

namespace FlagGG
{

AmbientOcclusionRenderingSoftware::AmbientOcclusionRenderingSoftware()
{

}

AmbientOcclusionRenderingSoftware::~AmbientOcclusionRenderingSoftware()
{

}

void AmbientOcclusionRenderingSoftware::RenderAO(const AmbientOcclusionInputData& inputData)
{
	AllocAOTexture(inputData.renderSolution_);

	// TODO:
}

Texture2D* AmbientOcclusionRenderingSoftware::GetAmbientOcclusionTexture() const
{
	return aoTexture_;
}

void AmbientOcclusionRenderingSoftware::AllocAOTexture(const IntVector2& renderSolution)
{
	if (!aoTexture_)
	{
		aoTexture_ = new Texture2D();
#if 0
		aoTexture_->SetSize(renderSolution.x_, renderSolution.y_, TEXTURE_FORMAT_R8, TEXTURE_RENDERTARGET);
#else
		// for test:
		aoTexture_->SetSize(1, 1, TEXTURE_FORMAT_R8, TEXTURE_RENDERTARGET);
		UInt8 occlusion = 255;
		aoTexture_->SetData(0, 0, 0, 1, 1, &occlusion);
#endif
	}
}

}
