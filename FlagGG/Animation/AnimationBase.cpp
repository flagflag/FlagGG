#include "Animation/AnimationBase.h"

namespace FlagGG
{

void AnimationTrack::AddPositionKeyFrame(float time, const Vector3& position)
{
	UInt32 keyIndex = positionKeyFrames_->CreateNewKey(time);
	positionKeyFrames_->SetKeyOut(0, keyIndex, position.x_);
	positionKeyFrames_->SetKeyOut(1, keyIndex, position.y_);
	positionKeyFrames_->SetKeyOut(2, keyIndex, position.z_);
}

void AnimationTrack::AddRotationKeyFrame(float time, const Quaternion& rotation)
{
	UInt32 keyIndex = rotationKeyFrames_->CreateNewKey(time);
	rotationKeyFrames_->SetKeyOut(0, keyIndex, rotation.w_);
	rotationKeyFrames_->SetKeyOut(1, keyIndex, rotation.x_);
	rotationKeyFrames_->SetKeyOut(2, keyIndex, rotation.y_);
	rotationKeyFrames_->SetKeyOut(3, keyIndex, rotation.z_);
}

void AnimationTrack::AddScaleKeyFrame(float time, const Vector3& scale)
{
	UInt32 keyIndex = scaleKeyFrames_->CreateNewKey(time);
	scaleKeyFrames_->SetKeyOut(0, keyIndex, scale.x_);
	scaleKeyFrames_->SetKeyOut(1, keyIndex, scale.y_);
	scaleKeyFrames_->SetKeyOut(2, keyIndex, scale.z_);
}

}
