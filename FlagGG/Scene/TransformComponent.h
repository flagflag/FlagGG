//
// 预留一个TrasnformComponent的文件，先把ITransformListener写在这里
//

#pragma once

namespace FlagGG
{

class ITransformListener
{
public:
	ITransformListener() {}

	virtual ~ITransformListener() {}

	// Trasnform发生改变
	virtual void OnTransformChange() = 0;

	// 位置发生变化
	virtual void OnPositionChange() = 0;

	// 旋转发生变化
	virtual void OnRotationChange() = 0;

	// 缩放发生变化
	virtual void OnScaleChange() = 0;
};

}
