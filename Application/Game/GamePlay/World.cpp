#include "GamePlay/World.h"
#include "EventDefine/GameEvent.h"

#include <Log.h>

World::World(Context* context) :
	context_(context)
{
	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, World::HandleUpdate, this));
}

void World::SetScene(Scene* scene)
{
	scene_ = scene;
}

Scene* World::GetScene() const
{
	return scene_;
}

Unit* World::CreateUnit(Int64 unitId)
{
	if (activeUnits_.Contains(unitId))
	{
		FLAGGG_LOG_WARN("Create unit who is in world.");
		return nullptr;
	}

	SharedPtr<Unit> unit;

	if (freeUnits_.Empty())
	{
		unit = new CEUnit(context_);
	}
	else
	{
		unit = freeUnits_.Front();
		freeUnits_.PopFront();
	}

	activeUnits_.Insert(MakePair(unitId, unit));
	scene_->AddChild(unit);

	context_->SendEvent<GameEvent::CREATE_UNIT_HANDLER>(unit);

	return unit;
}

void World::DestroyUnit(Int64 unitId)
{
	if (!activeUnits_.Contains(unitId))
	{
		FLAGGG_LOG_WARN("Destroy unit who is not in world.");
		return;
	}

	SharedPtr<Unit> currentUnit = activeUnits_[unitId];

	freeUnits_.Push(currentUnit);
	scene_->RemoveChild(currentUnit);

	context_->SendEvent<GameEvent::DESTROY_UNIT_HANDLER>(currentUnit);
}

Unit* World::GetUnit(Int64 unitId)
{
	auto it = activeUnits_.Find(unitId);
	if (it == activeUnits_.End())
		return nullptr;
	return it->second_;
}

Terrain* World::CreateTerrain()
{
	return nullptr;
}

void World::DestroyTerrain()
{

}

void World::HandleUpdate(float timeStep)
{
	for (auto it = activeUnits_.Begin(); it != activeUnits_.End(); ++it)
	{
		it->second_->OnUpdate(timeStep);
	}
}
