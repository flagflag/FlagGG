#include "Scene/AnimationComponent.h"
#include "Graphics/Model.h"

namespace FlagGG
{
	namespace Scene
	{
		void AnimationComponent::Update(float timeStep)
		{
			if (isPaused_) return;

			animTime_ += timeStep;
		}

		void AnimationComponent::SetAnimation(Animation* animtion)
		{
			animation_ = animtion;
		}

		void AnimationComponent::Play()
		{

		}

		void AnimationComponent::Stop()
		{

		}

		void AnimationComponent::Pause(bool pause)
		{
			isPaused_ = pause;
		}
	}
}
