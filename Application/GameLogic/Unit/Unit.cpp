#include <Scene/Component.h>

#include "Unit/Unit.h"

Unit::Unit(Context* context) :
	Node(),
	context_(context)
{ }

void Unit::Initialize()
{

}

// �ƶ���ĳ���㣬��SetPosition��ͬ���ǣ�������ٶ�������Ŀ�귽���ƶ�
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

