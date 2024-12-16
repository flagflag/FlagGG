#include "AnimationManager.h"
#include "Animation/AnimationAsset.h"
#include "Resource/ResourceCache.h"

namespace FlagGG
{

AnimationBase* AnimationManager::GetAnimation(const String& name)
{
	auto* anim = GetSubsystem<ResourceCache>()->GetResource<AnimationAsset>(name);
	return anim->GetAnimationData();
}

void AnimationManager::ClearAnimationCache()
{

}

}
