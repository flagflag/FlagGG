//
// 地表纹理权重
//

#pragma once

#include <Core/Object.h>
#include <Container/Ptr.h>
#include <Math/Vector2.h>

namespace FlagGG
{

class Texture2D;
class Image;

}

using namespace FlagGG;

class TerrainTextureWeights : public Object
{
	OBJECT_OVERRIDE(TerrainTextureWeights, Object);
public:
	struct TextureInfo
	{
		UInt32 id;
		Real w;
	};

	explicit TerrainTextureWeights();

	~TerrainTextureWeights() override;

	void SetSize(UInt32 width, UInt32 height);

	void SetTextureInfo(UInt32 x, UInt32 y, const TextureInfo* texInfo);

	void GetTextureInfo(UInt32 x, UInt32 y, TextureInfo* texInfo);

	void UpdateGpuTexture();

	Texture2D* GetIdTexture() const { return idTexture_; }

	Texture2D* GetWeightTexture() const { return weightTexture_; }

	IntVector2 GetSize() const;

protected:
	void DecodeFromImage(UInt32 x, UInt32 y, TextureInfo* texInfo);

	void EncodeToImage(UInt32 x, UInt32 y, const TextureInfo* texInfo);

	void EncodeToBuffer(UInt32 x, UInt32 y, const TextureInfo* texInfo);

	void EncodeIdToBuffer(UInt32 x, UInt32 y, UInt32 id0, UInt32 id1, UInt32 id2);

	void EncodeWeightToBuffer(UInt32 x, UInt32 y, float weight0, float weight1, float weight2);

	void DecodeFromBuffer(UInt32 x, UInt32 y, TextureInfo* texInfo);

private:
	// ID纹理
	SharedPtr<Texture2D> idTexture_;
	SharedPtr<Image> idImage_;
	PODVector<UInt32> idBuffer_;

	// 权重纹理
	SharedPtr<Texture2D> weightTexture_;
	SharedPtr<Image> weightImage_;
	PODVector<UInt32> weightBuffer_;
};
