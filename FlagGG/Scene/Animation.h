#pragma once

#include "Export.h"
#include "Resource/Resource.h"
#include "Container/HashMap.h"
#include "Container/StringHash.h"
#include "Container/Vector.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Animation/AnimationSequence.h"

namespace FlagGG
{

struct AnimationKeyFrame_Deprecated
{
	Real time_{ 0.0f };
	Vector3 position_{ Vector3::ZERO };
	Quaternion rotation_{ Quaternion::IDENTITY };
	Vector3 scale_{ Vector3::ONE };
};

struct FlagGG_API AnimationKeyFrameInterval_Deprecated
{
	Vector3 GetPosition(Real time);
	Quaternion GetRotation(Real time);
	Vector3 GetScale(Real time);

	const AnimationKeyFrame_Deprecated* left_{ nullptr };
	const AnimationKeyFrame_Deprecated* right_{ nullptr };
	Real timeInterval_{ 1e6 };
};

struct FlagGG_API AnimationTrack_Deprecated
{
	AnimationKeyFrameInterval_Deprecated GetKeyFrameInterval(Real currentTime, Real totalTime) const;

	String name_;
	StringHash nameHash_;
	AnimationChannelFlags channelMask_;
	PODVector<AnimationKeyFrame_Deprecated> keyFrames_;
};

class FlagGG_API Animation : public Resource
{
	OBJECT_OVERRIDE(Animation, Resource);
public:
	Animation();

	// 动画名字
	String name_;
	StringHash nameHash_;
	// 动画时长
	Real length_;
	// 动画轨迹
	HashMap<StringHash, AnimationTrack_Deprecated> tracks_;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;
};

}
