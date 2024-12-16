#include "Animation/AnimationSequence.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"
#include "Math/Distributions/DistributionQuaternionConstantCurve.h"

namespace FlagGG
{

AnimationSequence::AnimationSequence()
	: length_(0.0f)
{

}

void AnimationSequence::SetLength(float length)
{
	length_ = length;
}

UInt32 AnimationSequence::GetNumTracks() const
{
	return tracks_.Size();
}

const AnimationTrack* AnimationSequence::GetTrack(UInt32 index) const
{
	return index < tracks_.Size() ? &(tracks_[index]) : nullptr;
}

AnimationTrack& AnimationSequence::CreateOrGetAnimationTrack(const String& trackName)
{
	StringHash trackNameHash(trackName);

	auto it = nameToTrackMappings_.Find(trackNameHash);
	if (it != nameToTrackMappings_.End())
	{
		return tracks_[it->second_];
	}

	nameToTrackMappings_.Insert(MakePair(trackNameHash, tracks_.Size()));
	auto& track = tracks_.EmplaceBack();
	track.name_ = trackName;
	track.nameHash_ = trackNameHash;
	track.positionKeyFrames_ = new DistributionVectorConstantCurve();
	track.rotationKeyFrames_ = new DistributionQuaternionConstantCurve();
	track.scaleKeyFrames_ = new DistributionVectorConstantCurve();

	return track;
}

}
