#include <Scene/Component.h>

#include "Unit/Unit.h"

Unit::Unit(Context* context) :
	Node(),
	context_(context)
{ }

void Unit::Initialize()
{

}

// 移动到某个点，和SetPosition不同的是：会根据速度慢慢往目标方向移动
void Unit::MoveTo(const Vector3& position)
{

}

void Unit::SetSpeed(float speed)
{
	speed_ = speed;
}

float Unit::GetSpeed() const
{
	return speed_;
}

