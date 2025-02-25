#pragma once

#include "GameBuilder/Brush/BrushComponent.h"

namespace FlagGG
{

class Texture2D;
class Image;
class Node;
class RayQueryResult;

}

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

	void UpdateTextureWeight(const IntVector2& texPos, float blend);

	void UpdateGpuTexture();

	struct TextureInfo
	{
		Int32 id;
		Real w;
	};

	void DecodeFromImage(UInt32 x, UInt32 y, TextureInfo* texInfo);

	void EncodeToImage(UInt32 x, UInt32 y, const TextureInfo* texInfo);

	void EncodeToBuffer(UInt32 x, UInt32 y, const TextureInfo* texInfo);

private:
	bool isWorking_;

	IntVector2 lastFrameMousePos_;

	float brushSize_;
	Int32 brushId_;

	// ID纹理
	SharedPtr<Texture2D> idTexture_;
	SharedPtr<Image> idImage_;
	PODVector<UInt32> idBuffer_;

	// 权重纹理
	SharedPtr<Texture2D> weightTexture_;
	SharedPtr<Image> weightImage_;
	PODVector<UInt32> weightBuffer_;

	// 画刷HUD
	SharedPtr<Node> hudCircle_;
};
