#pragma once

#include "GamePlay/GamePlayBase.h"

#include <Container/Ptr.h>
#include <Graphics/Window.h>
#include <Scene/Camera.h>
#include <Scene/Node.h>

using namespace FlagGG::Graphics;
using namespace FlagGG::Scene;
using namespace FlagGG::Container;

class Perspective : public RefCounted
{
public:
	// 设置相机
	virtual void SetCamera(Camera* camera) = 0;
	virtual Camera* GetCamera() const = 0;

	// 设置所属窗口
	virtual void SetWindow(Window* window) = 0;
	virtual Window* GetWindow() const = 0;

	// 设置控制对象
	virtual void SetNode(Node* node) = 0;
	virtual Node* GetNode() const = 0;

	// 设置同步模式
	virtual void SetSyncMode(SyncMode syncMode) = 0;
	virtual SyncMode GetSyncMode() const = 0;

	// 重置视角
	virtual void Reset() = 0;

	// 锁定视角
	virtual void Lock() = 0;

	// 解锁视角
	virtual void Unlock() = 0;

private:

};
