#pragma once

#include "Unit/Unit.h"
#include "Unit/Terrain.h"

#include <Container/HashMap.h>
#include <Container/List.h>
#include <Scene/Scene.h>
#include <Core/Context.h>

class World : public RefCounted
{
public:
	World(Context* context);

	void SetScene(FlagGG::Scene::Scene* scene);

	FlagGG::Scene::Scene* GetScene() const;

	Unit* CreateUnit(Int64 unitId);

	void DestroyUnit(Int64 unitId);

	Unit* GetUnit(Int64 unitId);

	Terrain* CreateTerrain();

	void DestroyTerrain();

protected:
	void HandleUpdate(float timeStep);

private:
	Context* context_;

	HashMap<Int64, SharedPtr<Unit>> activeUnits_;

	List<SharedPtr<Unit>> freeUnits_;

	SharedPtr<Terrain> terrain_;

	SharedPtr<FlagGG::Scene::Scene> scene_;
};
