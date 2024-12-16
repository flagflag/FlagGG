//
// 动画层
//

#pragma once

#include "Container/Vector.h"
#include "Animation/AnimationState.h"

namespace FlagGG
{

struct AnimationLayer
{
	AnimationLayer(UInt32 layer);

	// 创建一个动画状态或从已有动画状态获取
	AnimationState* CreateOrGetAnimationState(AnimationBase* animationData);

	// 更新动画时间
	void Update(Real timeStep);

	// 应用当前time对应的骨骼Track姿态
	void ApplyTrack();

	// 动画层
	UInt32 layer_;
	// 动画状态（只有一个目标权重为1，其他都为0）
	Vector<SharedPtr<AnimationState>> animationStates_;
};

}
