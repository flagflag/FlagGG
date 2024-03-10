#include "Scene/Animation.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Resource/ResourceCache.h"
#include "Core/ObjectFactory.h"
#include "Log.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(Animation);

Vector3 AnimationKeyFrameInterval::GetPosition(Real time)
{
	if (left_ && right_)
	{
		if (Equals(timeInterval_, 0.0f))
			return left_->position_;
		Real t = (time - left_->time_) / timeInterval_;
		return left_->position_.Lerp(right_->position_, t);
	}

	return Vector3::ZERO;
}

Quaternion AnimationKeyFrameInterval::GetRotation(Real time)
{
	if (left_ && right_)
	{
		if (Equals(timeInterval_, 0.0f))
			return left_->rotation_;
		Real t = (time - left_->time_) / timeInterval_;
		return left_->rotation_.Slerp(right_->rotation_, t);
	}

	return Quaternion::IDENTITY;
}

Vector3 AnimationKeyFrameInterval::GetScale(Real time)
{
	if (left_ && right_)
	{
		if (Equals(timeInterval_, 0.0f))
			return left_->scale_;
		Real t = (time - left_->time_) / timeInterval_;
		return left_->scale_.Lerp(right_->scale_, t);
	}

	return Vector3::ONE;
}

AnimationKeyFrameInterval AnimationTrack::GetKeyFrameInterval(Real currentTime, Real totalTime) const
{
	AnimationKeyFrameInterval interval;
	Int32 index = 0;
	while (index < keyFrames_.Size() && currentTime > keyFrames_[index].time_) ++index;
	--index;
	if (index >= 0)
	{
		assert(currentTime > keyFrames_[index].time_);
		interval.left_ = &keyFrames_[index];
		index = (index + 1) % keyFrames_.Size();
		interval.right_ = &keyFrames_[index];
		interval.timeInterval_ = interval.right_->time_ - interval.left_->time_;
		if (interval.timeInterval_ < 0.0f)
		{
			interval.timeInterval_ += totalTime;
		}
	}
	return interval;
}

Animation::Animation() :
	Resource()
{ }

bool Animation::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String fileID;
	fileID.Resize(4);
	stream->ReadStream(&fileID[0], 4);
	if (fileID != "UANI")
	{
		FLAGGG_LOG_ERROR("invalid file id.");
		return false;
	}

	IOFrame::Buffer::ReadString(stream, name_);
	nameHash_ = StringHash(name_);
	stream->ReadFloat(length_);
	tracks_.Clear();

	UInt32 tracksCount = 0;
	stream->ReadUInt32(tracksCount);
	for (UInt32 i = 0; i < tracksCount; ++i) 
	{
		String trackName;
		IOFrame::Buffer::ReadString(stream, trackName);
		StringHash trackNameHash(trackName);
		AnimationTrack& track = tracks_[trackNameHash];
		track.name_ = trackName;
		track.nameHash_ = trackNameHash;

		uint8_t flag;
		stream->ReadUInt8(flag);
		track.channelMask_ = AnimationChannelFlags(flag);

		UInt32 keyFramesCount = 0;
		stream->ReadUInt32(keyFramesCount);
		track.keyFrames_.Resize(keyFramesCount);

		for (UInt32 j = 0; j < keyFramesCount; ++j)
		{
			AnimationKeyFrame& keyFrame = track.keyFrames_[j];
			stream->ReadFloat(keyFrame.time_);
			if (track.channelMask_ & AC_POSITION)
				IOFrame::Buffer::ReadVector3(stream, keyFrame.position_);
			if (track.channelMask_ & AC_ROTATION)
				IOFrame::Buffer::ReadQuaternion(stream, keyFrame.rotation_);
			if (track.channelMask_ & AC_SCALE)
				IOFrame::Buffer::ReadVector3(stream, keyFrame.scale_);
		}

		if (track.keyFrames_.Size() && !Equals(track.keyFrames_[0].time_, 0.0f))
		{
			auto keyFrame = track.keyFrames_[0];
			keyFrame.time_ = 0.0f;
			track.keyFrames_.Insert(track.keyFrames_.Begin(), keyFrame);
		}
	}

	return true;
}

bool Animation::EndLoad()
{
	return true;
}

}
