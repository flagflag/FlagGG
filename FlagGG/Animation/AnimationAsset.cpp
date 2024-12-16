#include "AnimationAsset.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Animation/AnimationSequence.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(AnimationAsset);

bool AnimationAsset::ReadAnimationSequence(IOFrame::Buffer::IOBuffer* stream)
{
	SharedPtr<AnimationSequence> animSeq(new AnimationSequence());

	IOFrame::Buffer::ReadString(stream, name_);
	nameHash_ = StringHash(name_);
	float length;
	stream->ReadFloat(length);
	animSeq->SetLength(length);

	UInt32 tracksCount = 0;
	stream->ReadUInt32(tracksCount);

	for (UInt32 i = 0; i < tracksCount; ++i)
	{
		String trackName;
		IOFrame::Buffer::ReadString(stream, trackName);
		AnimationTrack& track = animSeq->CreateOrGetAnimationTrack(trackName);

		uint8_t flag;
		stream->ReadUInt8(flag);
		track.channelMask_ = AnimationChannelFlags(flag);

		UInt32 keyFramesCount = 0;
		stream->ReadUInt32(keyFramesCount);

		for (UInt32 j = 0; j < keyFramesCount; ++j)
		{
			float time;
			stream->ReadFloat(time);

			if (track.channelMask_ & AC_POSITION)
			{
				Vector3 position;
				IOFrame::Buffer::ReadVector3(stream, position);
				if (j == 0 && !Equals(time, 0.0f))
					track.AddPositionKeyFrame(0.0f, position);
				track.AddPositionKeyFrame(time, position);
			}

			if (track.channelMask_ & AC_ROTATION)
			{
				Quaternion rotation;
				IOFrame::Buffer::ReadQuaternion(stream, rotation);
				if (j == 0 && !Equals(time, 0.0f))
					track.AddRotationKeyFrame(0.0f, rotation);
				track.AddRotationKeyFrame(time, rotation);
			}

			if (track.channelMask_ & AC_SCALE)
			{
				Vector3 scale;
				IOFrame::Buffer::ReadVector3(stream, scale);
				if (j == 0 && !Equals(time, 0.0f))
					track.AddScaleKeyFrame(0.0f, scale);
				track.AddScaleKeyFrame(time, scale);
			}
		}
	}

	animationData_ = animSeq;

	return true;
}

bool AnimationAsset::ReadAnimationTree(IOFrame::Buffer::IOBuffer* stream)
{
	return false;
}

bool AnimationAsset::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String fileID;
	fileID.Resize(4);
	stream->ReadStream(&fileID[0], 4);

	if (fileID == "UANI") // animation sequence
	{
		return ReadAnimationSequence(stream);
	}
	else if (fileID == "UATR") // animtion tree
	{
		return ReadAnimationTree(stream);
	}

	FLAGGG_LOG_ERROR("invalid file id.");

	return false;
}

}
