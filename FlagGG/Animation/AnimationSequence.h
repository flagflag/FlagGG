//
// 动画序列（动画关键曲线、动画事件）
//

#pragma once

#include "Resource/Resource.h"
#include "Animation/AnimationBase.h"
#include "Container/HashMap.h"
#include "Container/Vector.h"

namespace FlagGG
{

class AnimationSequence : public AnimationBase
{
	OBJECT_OVERRIDE(AnimationSequence, AnimationBase);
public:
	explicit AnimationSequence();

	// 获取动画时长
	float GetLength() const override { return length_; }

	// 设置动画时长
	void SetLength(float length);

	// 获取动画轨迹个数
	UInt32 GetNumTracks() const override;

	// 获取第index个动画轨迹
	const AnimationTrack* GetTrack(UInt32 index) const override;

	// 创建动画轨迹
	AnimationTrack& CreateOrGetAnimationTrack(const String& trackName);

private:
	// 动画时长
	Real length_;
	// 动画轨迹名到动画轨迹数组下标的映射
	HashMap<StringHash, UInt32> nameToTrackMappings_;
	// 动画轨迹
	Vector<AnimationTrack> tracks_;
};

}
