#pragma once

//
// 屏幕空间反射基类
//

#include "Core/Object.h"
#include "Math/Vector2.h"

namespace FlagGG
{

class Texture2D;
class Camera;

struct ScreenSpaceReflectionsInputData
{
	Texture2D* GBufferA_;
	Texture2D* GBufferB_;
	Texture2D* GBufferC_;
	Texture2D* screenDepthTexture_;
	Texture2D* HiZMap_;
	Camera* camera_;
	IntVector2 renderSolution_;
};

class IScreenSpaceReflections : public Object
{
	OBJECT_OVERRIDE(IScreenSpaceReflections, Object);
public:

	// 渲染SSR
	virtual void RenderSSR(const ScreenSpaceReflectionsInputData& inputData) = 0;

	// 获取渲染结果
	virtual Texture2D* GetGBufferSSR() const = 0;
};

}
