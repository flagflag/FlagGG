#include "Animation/AnimationLayers.h"
#include "Animation/AnimationBase.h"
#include "Math/Math.h"

namespace FlagGG
{

AnimationLayer::AnimationLayer(UInt32 layer)
	: layer_(layer)
{

}

AnimationState* AnimationLayer::CreateOrGetAnimationState(AnimationBase* animationData)
{
	for (auto& animState : animationStates_)
	{
		if (animState->GetAnimationData() == animationData)
			return animState;
	}

	SharedPtr<AnimationState> animState = MakeShared<AnimationState>(animationData);
	animationStates_.Push(animState);

	return animState;
}

void AnimationLayer::Update(Real timeStep)
{
	for (auto it = animationStates_.Begin(); it != animationStates_.End();)
	{
		auto animState = *it;

		animState->Update(timeStep);

		if ((Equals(animState->GetWeight(), 0.0f) && (Equals(animState->GetTargetWeight(), 0.0f) || Equals(animState->GetFadeTime(), 0.0f))) ||
			(!animState->GetLooped() && animState->GetTime() > animState->GetAnimationData()->GetLength()))
		{
			it = animationStates_.Erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AnimationLayer::ApplyTrack()
{
	for (auto& animState : animationStates_)
	{
		animState->ApplyTrack();
	}
}

}
