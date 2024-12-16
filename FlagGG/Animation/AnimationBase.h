//
// 动画基础
//

#pragma once

#include "Core/Object.h"
#include "Container/FlagSet.h"
#include "Math/Distributions/DistributionVector.h"
#include "Math/Distributions/DistributionQuaternion.h"

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

struct FlagGG_API AnimationTrack
{
	void AddPositionKeyFrame(float time, const Vector3& position);

	void AddRotationKeyFrame(float time, const Quaternion& rotation);

	void AddScaleKeyFrame(float time, const Vector3& scale);

	// 骨骼名字
	String name_;
	// 骨骼名字哈希
	StringHash nameHash_;
	// 关键帧Mask（AC_POSITION：有位置关键帧，AC_ROTATION：有旋转关键帧，AC_SCALE：有缩放关键帧）
	AnimationChannelFlags channelMask_;
	// 位置关键帧曲线
	SharedPtr<DistributionVector> positionKeyFrames_;
	// 旋转关键帧曲线
	SharedPtr<DistributionQuaternion> rotationKeyFrames_;
	// 缩放关键帧曲线
	SharedPtr<DistributionVector> scaleKeyFrames_;
};

class AnimationBase : public Object
{
	OBJECT_OVERRIDE(AnimationBase, Object);
public:
	// 获取动画时长
	virtual float GetLength() const = 0;

	// 获取动画轨迹个数
	virtual UInt32 GetNumTracks() const = 0;

	// 获取第index个动画轨迹
	virtual const AnimationTrack* GetTrack(UInt32 index) const = 0;
};

}
