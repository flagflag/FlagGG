#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>
#include <Container/Ptr.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

using namespace FlagGG::Core;
using namespace FlagGG::Scene;
using namespace FlagGG::Container;
using namespace FlagGG::Math;

class Unit : public Node
{
public:
	Unit(Context* context);

	void Initialize();

	// 移动到某个点，和SetPosition不同的是：会根据速度慢慢往目标方向移动
	void MoveTo(const Vector3& position);

	void SetSpeed(float speed);

	float GetSpeed() const;

protected:
	Context* context_;

	float speed_;
};
