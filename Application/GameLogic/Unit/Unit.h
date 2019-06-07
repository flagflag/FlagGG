#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>
#include <Container/Str.h>

using namespace FlagGG::Core;
using namespace FlagGG::Math;
using namespace FlagGG::Scene;
using namespace FlagGG::Container;

class Unit : public Node
{
public:
	Unit(Context* context);

	bool Load(const String& path);

	// �ƶ���ĳ���㣬��SetPosition��ͬ���ǣ�������ٶ�������Ŀ�귽���ƶ�
	void MoveTo(const Vector3& position);

	void SetSpeed(float speed);

	float GetSpeed() const;

	void PlayAnimation(const String& path, bool isLoop);

	void StopAnimation();

protected:
	Context* context_;

	float speed_;
};
