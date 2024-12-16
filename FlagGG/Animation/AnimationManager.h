//
// 动画管理
//

#pragma once

#include "Core/Subsystem.h"
#include "Container/Str.h"

namespace FlagGG
{

class AnimationBase;

class FlagGG_API AnimationManager : public Subsystem<AnimationManager>
{
public:
	// 获取动画
	AnimationBase* GetAnimation(const String& name);

	// 清除动画缓存
	void ClearAnimationCache();

private:

};

}
