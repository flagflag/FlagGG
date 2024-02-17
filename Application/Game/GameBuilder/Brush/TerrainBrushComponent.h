#include "GameBuilder/Brush/BrushComponent.h"

namespace FlagGG
{

class TerrainComponent;

}

class TerrainBrushComponent : public BrushComponent
{
	OBJECT_OVERRIDE(TerrainBrushComponent, BrushComponent);
public:
	TerrainBrushComponent();

	~TerrainBrushComponent() override;

// Override BrushComponent interface
	void OnAttach() override;

	void OnDetach() override;

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseMove(KeyState* keyState, const Vector2& delta) override;

	void OnWheel() override;

	void OnUpdate(float timeStep) override;

private:
	bool isWorking_;

	TerrainComponent* terrainComponent_;
};
