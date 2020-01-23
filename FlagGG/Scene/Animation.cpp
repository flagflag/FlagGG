#include "Scene/Animation.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Resource/ResourceCache.h"
#include "Log.h"

namespace FlagGG
{
	namespace Scene
	{
		Math::Vector3 AnimationKeyFrameInterval::GetPosition(Real time)
		{
			if (left_ && right_)
			{
				Real t = (time - left_->time_) / timeInterval_;
				return left_->position_.Lerp(right_->position_, t);
			}

			return Math::Vector3::ZERO;
		}

		Math::Quaternion AnimationKeyFrameInterval::GetRotation(Real time)
		{
			if (left_ && right_)
			{
				Real t = (time - left_->time_) / timeInterval_;
				return left_->rotation_.Slerp(right_->rotation_, t);
			}

			return Math::Quaternion::IDENTITY;
		}

		Math::Vector3 AnimationKeyFrameInterval::GetScale(Real time)
		{
			if (left_ && right_)
			{
				Real t = (time - left_->time_) / timeInterval_;
				return left_->scale_.Lerp(right_->scale_, t);
			}

			return Math::Vector3::ONE;
		}

		AnimationKeyFrameInterval AnimationTrack::GetKeyFrameInterval(Real currentTime, Real totalTime) const
		{
			AnimationKeyFrameInterval interval;
			UInt32 index = 0;
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

			UInt32 tracksCount = 0;
			stream->ReadUInt32(tracksCount);
			for (UInt32 i = 0; i < tracksCount; ++i) 
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
			}

			return true;
		}

		bool Animation::EndLoad()
		{
			return true;
		}
	}
}
