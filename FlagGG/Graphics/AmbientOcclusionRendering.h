//
// 环境光遮蔽渲染器
//

#pragma once

#include "Container/Ptr.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"

namespace FlagGG
{

class Texture2D;
class Camera;

struct AmbientOcclusionInputData
{
	Texture2D* screenNormalTexture_;
	Texture2D* screenDepthTexture_;
	Texture2D* HiZMap_;
	Camera* camera_;
	IntVector2 renderSolution_;
};

class AmbientOcclusionRendering : public RefCounted
{
public:
	virtual ~AmbientOcclusionRendering() {}

	// 渲染环境关遮蔽
	virtual void RenderAO(const AmbientOcclusionInputData& inputData) = 0;

	// 获取环境管遮蔽纹理
	virtual Texture2D* GetAmbientOcclusionTexture() const = 0;
};

}
