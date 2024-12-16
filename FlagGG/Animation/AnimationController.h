//
// 动画控制器
//

#pragma once

#include "Scene/Component.h"
#include "Animation/AnimationLayers.h"
#include "Container/HashMap.h"

namespace FlagGG
{

class FlagGG_API AnimationController : public Component
{
	OBJECT_OVERRIDE(AnimationController, Component);
public:
	AnimationController();

	~AnimationController() override;

	// 播放动画（硬切）
	void Play(const String& animName, UInt32 layer, bool looped, Real fadeTime = 0.0f, Real fadeOutTime = 0.0f);

	// 播放动画（硬切）
	void Play(AnimationBase* animationData, UInt32 layer, bool looped, Real fadeTime = 0.0f, Real fadeOutTime = 0.0f);

	// 播放动画（动画之间有过度）
	void PlaySmooth(const String& animName, UInt32 layer, bool looped, Real fadeTime = 0.0f, Real fadeOutTime = 0.0f);

	// 播放动画（动画之间有过度）
	void PlaySmooth(AnimationBase* animationData, UInt32 layer, bool looped, Real fadeTime = 0.0f, Real fadeOutTime = 0.0f);

	// 停止layer层动画
	void Stop(UInt32 layer, Real fadeOutTime = 0.0f);

	// 设置暂停
	void SetPause(bool paused);
	
protected:
	// 令layer层除了animationData的其他动画状态淡出
	void FadeOthers(AnimationBase* animationData, UInt32 layer, Real fadeOutTime);

	// 帧更新
	void Update(Real timeStep) override;

	// 更新动画（时间、权重）
	void UpdateAnimation(Real timeStep);

	// 混合动画轨迹
	void ApplyTrack();

	// 创建或者获取已存在的动画层
	AnimationLayer& CreateOrGetAnimationLayer(UInt32 layer);

	// 获取已经存的动画曾
	AnimationLayer* GetAnimationLayer(UInt32 layer) const;

private:
	// 动画层
	HashMap<UInt32, UniquePtr<AnimationLayer>> animationLayers_;
	// 内存缓存
	PODVector<AnimationLayer*> processLayers_;
	// 是否暂停的状态
	bool paused_;
};

}
