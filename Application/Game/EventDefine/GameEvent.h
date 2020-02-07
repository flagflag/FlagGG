#pragma once

#include <Core/EventDefine.h>
#include <Unit/Unit.h>
#include <Unit/Terrain.h>

namespace GameEvent
{
	DEFINE_EVENT(CREATE_UNIT, void(Unit*));
	DEFINE_EVENT(DESTROY_UNIT, void(Unit*));
	DEFINE_EVENT(CREATE_TERRAIN, void(Terrain*));
	DEFINE_EVENT(DESTROY_TERRAIN, void(Terrain*));
}
