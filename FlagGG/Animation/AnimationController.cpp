#include "Animation/AnimationController.h"
#include "Animation/AnimationManager.h"
#include "Animation/AnimationBase.h"
#include "Scene/Node.h"
#include "Scene/SkeletonMeshComponent.h"

namespace FlagGG
{

AnimationController::AnimationController()
	: paused_(false)
{

}

AnimationController::~AnimationController() = default;

void AnimationController::Play(const String& animName, UInt32 layer, bool looped, Real fadeTime, Real fadeOutTime)
{
	AnimationBase* animationData = GetSubsystem<AnimationManager>()->GetAnimation(animName);
	Play(animationData, layer, looped, fadeTime, fadeOutTime);
}

void AnimationController::PlaySmooth(const String& animName, UInt32 layer, bool looped, Real fadeTime, Real fadeOutTime)
{
	AnimationBase* animationData = GetSubsystem<AnimationManager>()->GetAnimation(animName);
	PlaySmooth(animationData, layer, looped, fadeTime, fadeOutTime);
}

void AnimationController::Stop(UInt32 layer, Real fadeOutTime)
{
	FadeOthers(nullptr, layer, fadeOutTime);
}

void AnimationController::SetPause(bool paused)
{
	paused_ = paused;
}

void AnimationController::Play(AnimationBase* animationData, UInt32 layer, bool looped, Real fadeTime, Real fadeOutTime)
{
	AnimationLayer& animLayer = CreateOrGetAnimationLayer(layer);
	AnimationState* animState = animLayer.CreateOrGetAnimationState(animationData);
	animState->SetWeight(0.0f);
	animState->SetTargetWeight(1.0f);
	animState->SetTime(0.0f);
	animState->SetFadeTime(fadeTime);
	animState->SetLooped(looped);
	
	auto* skeletonMeshComp = node_->GetComponent<SkeletonMeshComponent>();
	if (skeletonMeshComp)
	{
		animState->LinkSkeleton(skeletonMeshComp->GetSkeleton());
	}
}

void AnimationController::PlaySmooth(AnimationBase* animationData, UInt32 layer, bool looped, Real fadeTime, Real fadeOutTime)
{
	Play(animationData, layer, looped, fadeTime, fadeOutTime);
	FadeOthers(animationData, layer, fadeOutTime);
}

void AnimationController::FadeOthers(AnimationBase* animationData, UInt32 layer, Real fadeOutTime)
{
	if (auto* animLayer = GetAnimationLayer(layer))
	{
		for (auto& animState : animLayer->animationStates_)
		{
			if (animState->GetAnimationData() != animationData)
			{
				animState->SetTargetWeight(0.0f);
			}
		}
	}
}

void AnimationController::Update(Real timeStep)
{
	if (paused_)
		return;

	// 更新动画（时间、权重）
	UpdateAnimation(timeStep);

	// 混合动画轨迹
	ApplyTrack();
}

void AnimationController::UpdateAnimation(Real timeStep)
{
	for (auto& it : animationLayers_)
	{
		it.second_->Update(timeStep);
	}
}

void AnimationController::ApplyTrack()
{
	processLayers_.Clear();

	for (auto& it : animationLayers_)
	{
		processLayers_.Push(it.second_.Get());
	}

	// 按layer排序，混合时高级别layer覆盖低级别layer
	Sort(processLayers_.Begin(), processLayers_.End(), [](const AnimationLayer* _1, const AnimationLayer* _2)
	{
		return _1->layer_ < _2->layer_;
	});

	for (auto* animLayer : processLayers_)
	{
		animLayer->ApplyTrack();
	}

	node_->MarkDirty();
}

AnimationLayer& AnimationController::CreateOrGetAnimationLayer(UInt32 layer)
{
	if (auto* animationLayer = animationLayers_.TryGetValue(layer))
	{
		return *(*animationLayer);
	}

	auto& animLayer = animationLayers_[layer];
	animLayer.Reset(new AnimationLayer(layer));

	return *animLayer;
}

AnimationLayer* AnimationController::GetAnimationLayer(UInt32 layer) const
{
	auto* animationLayer = animationLayers_.TryGetValue(layer);
	return animationLayer ? animationLayer->Get() : nullptr;
}

}
