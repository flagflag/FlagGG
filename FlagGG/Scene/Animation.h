#pragma once

#include "Export.h"
#include "Resource/Resource.h"
#include "Container/FlagSet.h"
#include "Container/HashMap.h"
#include "Container/StringHash.h"
#include "Container/Vector.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace FlagGG
{
	namespace Scene
	{
		enum AnimationChannel : UInt8
		{
			AC_NONE = 0,
			AC_POSITION = 1 << 0,
			AC_ROTATION = 1 << 1,
			AC_SCALE = 1 << 2,
		};
	}

	namespace Container
	{
		FLAGGG_FLAGSET(Scene::AnimationChannel, AnimationChannelFlags);
	}

	namespace Scene
	{
		struct AnimationKeyFrame
		{
			Real time_{ 0.0f };
			Math::Vector3 position_{ Math::Vector3::ZERO };
			Math::Quaternion rotation_{ Math::Quaternion::IDENTITY };
			Math::Vector3 scale_{ Math::Vector3::ONE };
		};

		struct FlagGG_API AnimationKeyFrameInterval
		{
			Math::Vector3 GetPosition(Real time);
			Math::Quaternion GetRotation(Real time);
			Math::Vector3 GetScale(Real time);

			const AnimationKeyFrame* left_{ nullptr };
			const AnimationKeyFrame* right_{ nullptr };
			Real timeInterval_{ 1e6 };
		};

		struct FlagGG_API AnimationTrack
		{
			AnimationKeyFrameInterval GetKeyFrameInterval(Real currentTime, Real totalTime) const;

			Container::String name_;
			Container::StringHash nameHash_;
			Container::AnimationChannelFlags channelMask_;
			Container::PODVector<AnimationKeyFrame> keyFrames_;
		};

		class FlagGG_API Animation : public Resource::Resource
		{
		public:
			Animation(Core::Context* context);

			// 动画名字
			Container::String name_;
			Container::StringHash nameHash_;
			// 动画时长
			Real length_;
			// 动画轨迹
			Container::HashMap<Container::StringHash, AnimationTrack> tracks_;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;
		};
	}
}
