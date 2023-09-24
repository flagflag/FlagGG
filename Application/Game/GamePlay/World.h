#pragma once

#include "Unit/Unit.h"
#include "Unit/Terrain.h"

#include <Container/HashMap.h>
#include <Container/List.h>
#include <Scene/Scene.h>
#include <Core/Context.h>

class World : public Object
{
	OBJECT_OVERRIDE(World, Object);
public:
	World();

	void SetScene(Scene* scene);

	Scene* GetScene() const;

	Unit* CreateUnit(Int64 unitId);

	void DestroyUnit(Int64 unitId);

	Unit* GetUnit(Int64 unitId);

	Terrain* CreateTerrain();

	void DestroyTerrain();

protected:
	void HandleUpdate(float timeStep);

private:
	HashMap<Int64, SharedPtr<Unit>> activeUnits_;

	List<SharedPtr<Unit>> freeUnits_;

	SharedPtr<Terrain> terrain_;

	SharedPtr<Scene> scene_;
};
