#include "Animation/AnimationState.h"
#include "Animation/AnimationBase.h"
#include "Scene/Bone.h"

namespace FlagGG
{

AnimationState::AnimationState(AnimationBase* animationData)
	: animationData_(animationData)
	, linkSkeleton_(nullptr)
	, time_(0.0f)
	, weight_(0.0f)
	, targetWeight_(0.0f)
	, fadeTime_(0.0f)
	, looped_(false)
{

}

AnimationState::~AnimationState()
{

}

void AnimationState::SetWeight(Real weight)
{
	weight_ = weight;
}

void AnimationState::SetTargetWeight(Real targetWeight)
{
	targetWeight_ = targetWeight;
}

void AnimationState::SetTime(Real time)
{
	time_ = time;
}

void AnimationState::SetFadeTime(Real fadeTime)
{
	fadeTime_ = fadeTime;
}

void AnimationState::SetLooped(bool looped)
{
	looped_ = looped;
}

void AnimationState::LinkSkeleton(const Skeleton& skeleton)
{
	if (linkSkeleton_ == &skeleton)
		return;

	auto* rootBone = skeleton.GetRootBone();

	if (!rootBone)
	{
		FLAGGG_LOG_WARN("Try to set animation track, but root bone is nullptr.");
		return;
	}

	for (UInt32 trackIndex = 0; trackIndex < animationData_->GetNumTracks(); ++trackIndex)
	{
		const AnimationTrack* animTrack = animationData_->GetTrack(trackIndex);
		BoneTrack boneTrack;
		boneTrack.track_ = animTrack;
		if (rootBone->nameHash_ == animTrack->nameHash_)
		{
			boneTrack.bone_ = rootBone;
		}
		else
		{
			Node* boneNode = rootBone->node_->GetChild(animTrack->nameHash_, true);
			if (boneNode)
			{
				boneTrack.bone_ = skeleton.GetBone(animTrack->nameHash_);
			}
		}

		if (boneTrack.bone_ && boneTrack.bone_->node_)
		{
			boneTracks_.Push(boneTrack);
		}
	}
}

void AnimationState::Update(Real timeStep)
{
	time_ += timeStep;
	if (time_ > animationData_->GetLength())
	{
		if (looped_)
		{
			time_ -= animationData_->GetLength();
		}
	}

	if (!Equals(fadeTime_, 0.0f))
	{
		Real weightDelta = 1.0f / fadeTime_ * timeStep;
		if (weight_ < targetWeight_)
			weight_ = Min(weight_ + weightDelta, targetWeight_);
		else if (weight_ > targetWeight_)
			weight_ = Max(weight_ - weightDelta, targetWeight_);
	}
	else
	{
		weight_ = targetWeight_;
	}
}

void AnimationState::ApplyTrack()
{
	for (auto& boneTrack : boneTracks_)
	{
		UpdateBoneTrack(boneTrack);
	}
}

void AnimationState::UpdateBoneTrack(const BoneTrack& boneTrack)
{
	Node* boneNode = boneTrack.bone_->node_;
	bool lerpValue = !Equals(weight_, 1.0f);

	if (boneTrack.track_->channelMask_ & AC_POSITION)
	{
		Vector3 position = boneTrack.track_->positionKeyFrames_->GetVectorValue(time_);
		if (lerpValue)
			position = boneNode->GetPosition().Lerp(position, weight_);
		boneNode->SetPositionSilent(position);
	}

	if (boneTrack.track_->channelMask_ & AC_ROTATION)
	{
		Quaternion rotation = boneTrack.track_->rotationKeyFrames_->GetQuatValue(time_);
		if (lerpValue)
			rotation = boneNode->GetRotation().Slerp(rotation, weight_);
		boneNode->SetRotationSilent(rotation);
	}

	if (boneTrack.track_->channelMask_ & AC_SCALE)
	{
		Vector3 scale = boneTrack.track_->scaleKeyFrames_->GetVectorValue(time_);
		if (lerpValue)
			scale = boneNode->GetScale().Lerp(scale, weight_);
		boneNode->SetScaleSilent(scale);
	}
}

}
