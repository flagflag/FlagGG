#include "Scene/Component.h"
#include "Scene/Animation.h"
#include "Container/Ptr.h"

namespace FlagGG
{
	namespace Scene
	{
		class AnimationComponent : public Component
		{
		public:
			void Update(float timeStep) override;

			void SetAnimation(Animation* animtion);

			void Play();

			void Stop();

			void Pause(bool pause);

		private:
			Container::SharedPtr<Animation> animation_;

			float animTime_{ 0.0f };

			bool isPaused_{ false };
		};
	}
}
