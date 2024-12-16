//
// 动画资产
//

#pragma once

#include "Resource/Resource.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class AnimationBase;

class AnimationAsset : public Resource
{
	OBJECT_OVERRIDE(AnimationAsset, Resource);
public:
	// 获取动画数据
	AnimationBase* GetAnimationData() const { return animationData_; }

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	// 读取动画序列
	bool ReadAnimationSequence(IOFrame::Buffer::IOBuffer* stream);

	// 读取动画树
	bool ReadAnimationTree(IOFrame::Buffer::IOBuffer* stream);

private:
	// 动画名字
	String name_;
	// 动画名字哈希
	StringHash nameHash_;
	// 动画轨迹数据
	SharedPtr<AnimationBase> animationData_;
};

}
