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

enum AnimationChannel : UInt8
{
	AC_NONE = 0,
	AC_POSITION = 1 << 0,
	AC_ROTATION = 1 << 1,
	AC_SCALE = 1 << 2,
};

FLAGGG_FLAGSET(AnimationChannel, AnimationChannelFlags);

struct AnimationKeyFrame
{
	Real time_{ 0.0f };
	Vector3 position_{ Vector3::ZERO };
	Quaternion rotation_{ Quaternion::IDENTITY };
	Vector3 scale_{ Vector3::ONE };
};

struct FlagGG_API AnimationKeyFrameInterval
{
	Vector3 GetPosition(Real time);
	Quaternion GetRotation(Real time);
	Vector3 GetScale(Real time);

	const AnimationKeyFrame* left_{ nullptr };
	const AnimationKeyFrame* right_{ nullptr };
	Real timeInterval_{ 1e6 };
};

struct FlagGG_API AnimationTrack
{
	AnimationKeyFrameInterval GetKeyFrameInterval(Real currentTime, Real totalTime) const;

	String name_;
	StringHash nameHash_;
	AnimationChannelFlags channelMask_;
	PODVector<AnimationKeyFrame> keyFrames_;
};

class FlagGG_API Animation : public Resource
{
public:
	Animation(Context* context);

	// 动画名字
	String name_;
	StringHash nameHash_;
	// 动画时长
	Real length_;
	// 动画轨迹
	HashMap<StringHash, AnimationTrack> tracks_;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;
};

}
