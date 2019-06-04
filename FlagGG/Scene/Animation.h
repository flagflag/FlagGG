#pragma once

#include "Export.h"
#include "Resource/Resource.h"
#include "Container/FlagSet.h"
#include "Container/HashMap.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace FlagGG
{
	namespace Scene
	{
		enum AnimationChannel : uint8_t
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
			float time_{ 0.0f };
			Math::Vector3 position_{ Math::Vector3::ZERO };
			Math::Quaternion rotation_{ Math::Quaternion::IDENTITY };
			Math::Vector3 scale_{ Math::Vector3::ONE };
		};

		struct FlagGG_API AnimationTrack
		{
			AnimationKeyFrame* GetKeyFrame(float time) const;

			Container::String name_;
			Container::AnimationChannelFlags channelMask_{};
			Container::Vector<AnimationKeyFrame> keyFrames_;
		};

		class FlagGG_API Animation : public Resource::Resource
		{
		public:
			Animation(Core::Context* context);

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

		private:
			// 动画名字
			Container::String name_;
			// 动画时长
			float length_;
			// 动画轨迹
			Container::HashMap<Container::String, AnimationTrack> tracks_;
		};
	}
}
