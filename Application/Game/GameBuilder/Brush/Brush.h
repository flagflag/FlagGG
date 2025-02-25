#pragma once

#include <Core/Object.h>
#include <Core/DeviceEvent.h>
#include <Container/Ptr.h>

namespace FlagGG
{
	
class Scene;

}

using namespace FlagGG;

class BrushComponent;

class Brush : public Object
{
	OBJECT_OVERRIDE(Brush, Object);
public:
	Brush(Scene* scene);

	~Brush() override;

	void AttachComponent(BrushComponent* brushComponent);

	void DetachComponent(BrushComponent* brushComponent);

	BrushComponent* GetComponent(StringHash className);

	Scene* GetOwnerScene() const { return scene_; }

protected:
	void OnMouseDown(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnMouseMove(KeyState* keyState, const IntVector2& mousePos, const Vector2& delta);

	void OnWheel();

	void OnKeyDown(KeyState* keyState, UInt32 keyCode);

	void OnKeyUp(KeyState* keyState, UInt32 keyCode);

	void OnUpdate(float timeStep);
	
private:
	Scene* scene_;

	Vector<SharedPtr<BrushComponent>> brushComponents_;
};