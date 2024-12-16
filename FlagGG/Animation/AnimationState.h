//
// 动画状态
//

#pragma once

#include "Container/Ptr.h"
#include "Container/Vector.h"

namespace FlagGG
{

class AnimationBase;
struct Bone;
class Skeleton;
struct AnimationTrack;

// POD data
struct BoneTrack
{
	const Bone* bone_;
	const AnimationTrack* track_;
};

class AnimationState : public RefCounted
{
public:
	AnimationState(AnimationBase* animationData);

	~AnimationState() override;

	// 设置动画权重
	void SetWeight(Real weight);

	// 获取动画权重
	Real GetWeight() const { return weight_; }

	// 设置目标权重
	void SetTargetWeight(Real targetWeight);

	// 获取目标权重
	Real GetTargetWeight() const { return targetWeight_; }

	// 设置动画时间
	void SetTime(Real time);

	// 获取动画时间
	Real GetTime() const { return time_; }

	// 设置过度时间
	void SetFadeTime(Real fadeTime);

	// 获取过度时间
	Real GetFadeTime() const { return fadeTime_; }

	// 设置是否循环播放
	void SetLooped(bool looped);

	// 获取是否循环播放
	bool GetLooped() const { return looped_; }

	// 关联骨架
	void LinkSkeleton(const Skeleton& skeleton);

	// 更新动画时间
	void Update(Real timeStep);

	// 应用当前time对应的骨骼Track姿态
	void ApplyTrack();

	// 获取动画数据
	AnimationBase* GetAnimationData() const { return animationData_; }

protected:
	void UpdateBoneTrack(const BoneTrack& boneTrack);

private:
	// 动画数据
	SharedPtr<AnimationBase> animationData_;
	// 关联的骨架（只用于校验dirty，实际不会使用指针，有可能组件会被删除）
	Skeleton* linkSkeleton_;
	// 当前动画对应的骨骼轨迹
	PODVector<BoneTrack> boneTracks_;
	// 动画时间
	Real time_;
	// 动画权重
	Real weight_;
	// 动画目标权重
	Real targetWeight_;
	// 过度时间
	Real fadeTime_;
	// 是否循环播放
	bool looped_;
};

}
