#include "Scene/Animation.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Resource/ResourceCache.h"
#include "Log.h"

namespace FlagGG
{
	namespace Scene
	{
		AnimationKeyFrame* AnimationTrack::GetKeyFrame(float time) const
		{
			return nullptr;
		}

		Animation::Animation(Core::Context* context) :
			Resource(context)
		{ }

		bool Animation::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Container::String fileID;
			fileID.Resize(4);
			stream->ReadStream(&fileID[0], 4);
			if (fileID != "UANI")
			{
				FLAGGG_LOG_ERROR("invalid file id.");
				return false;
			}

			IOFrame::Buffer::ReadString(stream, name_);
			nameHash_ = Container::StringHash(name_);
			stream->ReadFloat(length_);
			tracks_.Clear();

			uint32_t tracksCount = 0;
			stream->ReadUInt32(tracksCount);
			for (uint32_t i = 0; i < tracksCount; ++i) 
			{
				Container::String trackName;
				IOFrame::Buffer::ReadString(stream, trackName);
				Container::StringHash trackNameHash(trackName);
				AnimationTrack& track = tracks_[trackNameHash];
				track.name_ = trackName;
				track.nameHash_ = trackNameHash;

				uint8_t flag;
				stream->ReadUInt8(flag);
				track.channelMask_ = Container::AnimationChannelFlags(flag);

				uint32_t keyFramesCount = 0;
				stream->ReadUInt32(keyFramesCount);
				track.keyFrames_.Resize(keyFramesCount);

				for (uint32_t j = 0; j < keyFramesCount; ++j)
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
			}

			return true;
		}

		bool Animation::EndLoad()
		{
			return true;
		}
	}
}
