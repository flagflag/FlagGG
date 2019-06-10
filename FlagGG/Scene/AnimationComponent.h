#include "Scene/Component.h"
#include "Scene/Animation.h"
#include "Scene/Bone.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

namespace FlagGG
{
	namespace Scene
	{
		struct BoneTrack
		{
			Bone* bone_{ nullptr };
			const AnimationTrack* track_{ nullptr };
		};

		class AnimationComponent : public Component
		{
		public:
			void Update(float timeStep) override;

			void SetAnimation(Animation* animation);

			void Play(bool isLoop);

			void Stop();

			void Pause(bool pause);

		protected:
			void SetAnimationTrack();

			void UpdateAnimation(float timeStep);

			void UpdateBoneTrack(BoneTrack& boneTrack);

		private:
			Container::SharedPtr<Animation> animation_;
			Container::PODVector<BoneTrack> boneTracks_;

			float animTime_{ 0.0f };
			bool startPlay_{ false };
			bool isPaused_{ false };
			bool isLoop_{ false };
		};
	}
}
