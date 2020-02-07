#include "Scene/AnimationComponent.h"
#include "Scene/SkeletonMeshComponent.h"
#include "Graphics/Model.h"
#include "Log.h"

namespace FlagGG
{
	namespace Scene
	{
		void AnimationComponent::Update(Real timeStep)
		{
			if (startPlay_ && !isPaused_)
			{
				UpdateAnimation(timeStep);
			}
		}

		void AnimationComponent::UpdateAnimation(Real timeStep)
		{
			animTime_ += timeStep;

			if (animTime_ > animation_->length_)
			{
				if (isLoop_)
				{
					animTime_ -= animation_->length_;
				}
				else
				{
					animTime_ = animation_->length_;
				}
			}

			for (auto& boneTrack : boneTracks_)
			{
				UpdateBoneTrack(boneTrack);
			}

			node_->UpdateTreeDirty();
		}

		void AnimationComponent::UpdateBoneTrack(BoneTrack& boneTrack)
		{
			AnimationKeyFrameInterval keyFrameInterval = boneTrack.track_->GetKeyFrameInterval(animTime_, animation_->length_);
			const Container::AnimationChannelFlags channelMask = boneTrack.track_->channelMask_;
			if (channelMask & AC_POSITION)
			{
				boneTrack.bone_->node_->SetPosition(keyFrameInterval.GetPosition(animTime_));
			}
			if (channelMask & AC_ROTATION)
			{
				boneTrack.bone_->node_->SetRotation(keyFrameInterval.GetRotation(animTime_));
			}
			if (channelMask & AC_SCALE)
			{
				boneTrack.bone_->node_->SetScale(keyFrameInterval.GetScale(animTime_));
			}
		}

		void AnimationComponent::SetAnimation(Animation* animation)
		{
			if (animation_ != animation)
			{
				animation_ = animation;

				SetAnimationTrack();
			}
		}

		void AnimationComponent::SetAnimationTrack()
		{
			// Í£Ö¹¾ÉµÄ¶¯»­
			Stop();

			boneTracks_.Clear();

			if (!animation_)
			{
				FLAGGG_LOG_WARN("Try to set animation track, but animation is nullptr.");
				return;
			}

			auto* skeletonMeshComp = node_->GetComponent<SkeletonMeshComponent>();
			if (!skeletonMeshComp)
			{
				FLAGGG_LOG_WARN("Try to set animation track, but owner node not contains skeleton mesh component.");
				return;
			}

			auto& skeleton = skeletonMeshComp->GetSkeleton();
			auto* rootBone = skeleton.GetRootBone();

			if (!rootBone)
			{
				FLAGGG_LOG_WARN("Try to set animation track, but root bone is nullptr.");
				return;
			}

			for (const auto& it : animation_->tracks_)
			{
				const AnimationTrack& animTrack = it.second_;
				BoneTrack boneTrack;
				boneTrack.track_ = &animTrack;
				if (rootBone->nameHash_ == animTrack.nameHash_)
				{
					boneTrack.bone_ = rootBone;
				}
				else
				{
					Node* boneNode = rootBone->node_->GetChild(animTrack.nameHash_, true);
					if (boneNode)
					{
						boneTrack.bone_ = skeleton.GetBone(animTrack.nameHash_);
					}
				}

				if (boneTrack.bone_ && boneTrack.bone_->node_)
				{
					boneTracks_.Push(boneTrack);
				}
			}
		}

		void AnimationComponent::Play(bool isLoop)
		{
			animTime_ = 0.0f;
			startPlay_ = true;
			isPaused_ = false;
			isLoop_ = isLoop;
		}

		void AnimationComponent::Stop()
		{
			animTime_ = 0.0f;
			startPlay_ = false;
			isPaused_ = false;
			isLoop_ = false;
		}

		void AnimationComponent::Pause(bool pause)
		{
			isPaused_ = pause;
		}
	}
}
