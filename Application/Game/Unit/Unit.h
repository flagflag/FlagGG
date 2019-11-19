#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>
#include <Graphics/Material.h>
#include <Container/Str.h>

using namespace FlagGG::Core;
using namespace FlagGG::Math;
using namespace FlagGG::Scene;
using namespace FlagGG::Graphics;
using namespace FlagGG::Container;

class Unit : public Node
{
public:
	Unit(Context* context);

	bool Load(const String& path);

	// 移动到某个点，和SetPosition不同的是：会根据速度慢慢往目标方向移动
	void MoveTo(const Vector3& position);

	void SetSpeed(float speed);

	float GetSpeed() const;

	void PlayAnimation(const String& path, bool isLoop);

	void StopAnimation();

	Material* GetMaterial() const;

protected:
	Context* context_;

	Material* material_;

	float speed_;
};
