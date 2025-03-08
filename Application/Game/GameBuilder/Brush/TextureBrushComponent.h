//
// 纹理混合笔刷
//

#pragma once

#include "GameBuilder/Brush/BrushComponent.h"

namespace FlagGG
{

class Texture2D;
class Image;
class Node;
class RayQueryResult;

}

class TerrainTextureWeights;

class TextureBrushComponent : public BrushComponent
{
	OBJECT_OVERRIDE(TextureBrushComponent, BrushComponent);
public:
	TextureBrushComponent();

	~TextureBrushComponent() override;

	// Override BrushComponent interface
	void OnAttach() override;

	void OnDetach() override;

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey) override;

	void OnMouseMove(KeyState* keyState, const Vector2& delta) override;

	void OnWheel() override;

	void OnKeyUp(KeyState* keyState, UInt32 keyCode) override;

	void OnUpdate(float timeStep) override;

	void SetBrushId(UInt32 brushId);

protected:
	void TryCreateData();

	void UpdateTextureWeight(const RayQueryResult& result);

	void UpdateTextureWeight3(const IntVector2& texPos, float blend);

	void UpdateTextureWeight2(const IntVector2& texPos, float blend);

private:
	bool isWorking_;

	IntVector2 lastFrameMousePos_;

	float brushSize_;
	Int32 brushId_;

	// 地表纹理权重
	SharedPtr<TerrainTextureWeights> texWeights_;

	// 画刷HUD
	SharedPtr<Node> hudCircle_;
};
