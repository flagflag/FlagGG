#include "TerrainTextureWeights.h"

#include <Resource/Image.h>
#include <Graphics/Texture2D.h>
#include <TypeTraits/IsArray.h>
#include <Math/Math.h>

#define BLEND_TEST 0
#define SPLAT_MAP3 1

TerrainTextureWeights::TerrainTextureWeights()
{

}

TerrainTextureWeights::~TerrainTextureWeights()
{

}

void TerrainTextureWeights::SetSize(UInt32 width, UInt32 height)
{
	idImage_ = new Image();
	idImage_->SetSize(width, height, 4);
	for (Int32 x = 0; x < idImage_->GetWidth(); ++x)
	{
		for (Int32 y = 0; y < idImage_->GetHeight(); ++y)
		{
			idImage_->SetPixel(x, y, Color(2.0 / 255, 255, 2.0 / 255.0));
		}
	}

	weightImage_ = new Image();
	weightImage_->SetSize(width, height, 4);
	for (Int32 x = 0; x < weightImage_->GetWidth(); ++x)
	{
		for (Int32 y = 0; y < weightImage_->GetHeight(); ++y)
		{
			weightImage_->SetPixel(x, y, Color(0.0, 0.0, 1.0));
		}
	}

	idTexture_ = new Texture2D();
	idTexture_->SetNumLevels(1);
	idTexture_->SetFilterMode(TEXTURE_FILTER_NEAREST);
	idTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
	idTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
	idTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
#if SPLAT_MAP3
	idTexture_->SetSize(width * 3, height * 3, TEXTURE_FORMAT_RGBA8);
#else
	idTexture_->SetSize(width, height, TEXTURE_FORMAT_RGBA8);
#endif

	weightTexture_ = new Texture2D();
	weightTexture_->SetNumLevels(1);
	weightTexture_->SetFilterMode(TEXTURE_FILTER_BILINEAR);
	weightTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
	weightTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
	weightTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
#if SPLAT_MAP3
	weightTexture_->SetSize(width * 3, height * 3, TEXTURE_FORMAT_RGBA8);
#else
	weightTexture_->SetSize(width * 2, height * 2, TEXTURE_FORMAT_RGBA8);
#endif

	UpdateGpuTexture();
}

IntVector2 TerrainTextureWeights::GetSize() const
{
	return IntVector2(idImage_->GetWidth(), idImage_->GetHeight());
}

void TerrainTextureWeights::SetTextureInfo(UInt32 x, UInt32 y, const TextureInfo* texInfo)
{
	EncodeToImage(x, y, texInfo);
}

void TerrainTextureWeights::GetTextureInfo(UInt32 x, UInt32 y, TextureInfo* texInfo)
{
	DecodeFromImage(x, y, texInfo);
}

void TerrainTextureWeights::DecodeFromImage(UInt32 x, UInt32 y, TextureInfo* texInfo)
{
	const Color color = idImage_->GetPixel(x, y);
	const Color weight = weightImage_->GetPixel(x, y);

	texInfo[0] = { static_cast<UInt32>(FloorToInt(color.r_ * 255.0 + 0.5)), weight.r_ };
	texInfo[1] = { static_cast<UInt32>(FloorToInt(color.g_ * 255.0 + 0.5)), weight.g_ };
	texInfo[2] = { static_cast<UInt32>(FloorToInt(color.b_ * 255.0 + 0.5)), weight.b_ };
}

void TerrainTextureWeights::EncodeToImage(UInt32 x, UInt32 y, const TextureInfo* texInfo)
{
	idImage_->SetPixel(x, y, Color(texInfo[0].id / 255.0, texInfo[1].id / 255.0, texInfo[2].id / 255.0));
	weightImage_->SetPixel(x, y, Color(texInfo[0].w, texInfo[1].w, texInfo[2].w));
}

void TerrainTextureWeights::EncodeToBuffer(UInt32 x, UInt32 y, const TextureInfo* texInfo)
{
	idBuffer_[y * idTexture_->GetWidth() + x] = Color(texInfo[0].id / 255.0, texInfo[1].id / 255.0, texInfo[2].id / 255.0).ToUInt();
	weightBuffer_[y * weightTexture_->GetWidth() + x] = Color(texInfo[0].w, texInfo[1].w, texInfo[2].w).ToUInt();
}

void TerrainTextureWeights::EncodeIdToBuffer(UInt32 x, UInt32 y, UInt32 id0, UInt32 id1, UInt32 id2)
{
	idBuffer_[y * idTexture_->GetWidth() + x] = Color(id0 / 255.0, id1 / 255.0, id2 / 255.0).ToUInt();
}

void TerrainTextureWeights::EncodeWeightToBuffer(UInt32 x, UInt32 y, float weight0, float weight1, float weight2)
{
	weightBuffer_[y * weightTexture_->GetWidth() + x] = Color(weight0, weight1, weight2).ToUInt();
}

void TerrainTextureWeights::DecodeFromBuffer(UInt32 x, UInt32 y, TextureInfo* texInfo)
{
	Color color;
	color.FromUInt(idBuffer_[y * idTexture_->GetWidth() + x]);
	texInfo[0].id = Floor(color.r_ * 255.0 + 0.5);
	texInfo[1].id = Floor(color.g_ * 255.0 + 0.5);
	texInfo[2].id = Floor(color.b_ * 255.0 + 0.5);

	color.FromUInt(weightBuffer_[y * weightTexture_->GetWidth() + x] = Color(texInfo[0].w, texInfo[1].w, texInfo[2].w).ToUInt());
	texInfo[0].w = color.r_;
	texInfo[1].w = color.g_;
	texInfo[2].w = color.b_;
}

void TerrainTextureWeights::UpdateGpuTexture()
{
	TextureInfo srcTex[3];
	TextureInfo dstTex[3];

	IntVector2 direction[] =
	{
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
#if SPLAT_MAP3
		{ -1, 1 },
		{ -1, 0 },
		{ -1, -1 },
		{ 0, -1 },
		{ 1, -1 },
#endif
	};
	const UInt32 dirCount = ARRAY_COUNT(direction);

	idBuffer_.Resize(idTexture_->GetWidth() * idTexture_->GetHeight());
	weightBuffer_.Resize(weightTexture_->GetWidth() * weightTexture_->GetHeight());

	for (Int32 x = 0; x < idImage_->GetWidth(); ++x)
	{
		for (Int32 y = 0; y < idImage_->GetHeight(); ++y)
		{
			DecodeFromImage(x, y, srcTex);
#if SPLAT_MAP3
			EncodeToBuffer(x * 3 + 1, y * 3 + 1, srcTex);
#else
			EncodeIdToBuffer(x, y, srcTex[0].id, srcTex[1].id, srcTex[2].id);
			EncodeWeightToBuffer(x * 2, y * 2, srcTex[0].w, srcTex[1].w, srcTex[2].w);
#endif

			for (UInt32 k = 0; k < dirCount; ++k)
			{
				Int32 nx = Clamp(x + direction[k].x_, 0, idImage_->GetWidth() - 1);
				Int32 ny = Clamp(y + direction[k].y_, 0, idImage_->GetHeight() - 1);

				DecodeFromImage(nx, ny, dstTex);

				TextureInfo nxtTex[3];
				nxtTex[0].id = 2;
				nxtTex[0].w = 0;

				for (Int32 i = 1; i < 3; ++i)
				{
					nxtTex[i].id = srcTex[i].id;
					nxtTex[i].w = 0.0f;

					for (Int32 j = 2; j >= 1; --j)
					{
						if (srcTex[i].id == dstTex[j].id)
						{
#if SPLAT_MAP3
							nxtTex[i].w = (srcTex[i].w + dstTex[j].w) * 0.5f;
#else
							nxtTex[i].w = dstTex[j].w;
#endif
							break;
						}
					}
				}

#if SPLAT_MAP3
				if (!Equals(nxtTex[1].w + nxtTex[2].w, 1.0f))
				{
					nxtTex[0].w = 1.0 - nxtTex[1].w - nxtTex[2].w;
				}
				EncodeToBuffer(x * 3 + 1 + direction[k].x_, y * 3 + 1 + direction[k].y_, nxtTex);
#else
				EncodeWeightToBuffer(x * 2 + direction[k].x_, y * 2 + direction[k].y_, nxtTex[0].w, nxtTex[1].w, nxtTex[2].w);
#endif
			}
		}
	}

#if SPLAT_MAP3
	float idMaxWeight[256];
	float idMinWeight[256];
	Int32 idCount[256];
	for (Int32 i = 0; i < 256; ++i)
	{
		idMaxWeight[i] = 0;
		idMinWeight[i] = 666;
		idCount[i] = 0;
	}

	for (Int32 x = 0; x < idImage_->GetWidth(); ++x)
	{
		for (Int32 y = 0; y < idImage_->GetHeight(); ++y)
		{
			if (x < idImage_->GetWidth() - 1 && y < idImage_->GetHeight() - 1)
			{
				IntVector2 texcoord[4] =
				{
					IntVector2(x * 3 + 2, y * 3 + 2),
					IntVector2(x * 3 + 3, y * 3 + 2),
					IntVector2(x * 3 + 2, y * 3 + 3),
					IntVector2(x * 3 + 3, y * 3 + 3),
				};
				TextureInfo tex[4][3];

				for (UInt32 i = 0; i < 4; ++i)
				{
					DecodeFromBuffer(texcoord[i].x_, texcoord[i].y_, tex[i]);
					for (UInt32 j = 1; j < 3; ++j)
					{
						idMaxWeight[tex[i][j].id] = Max(idMaxWeight[tex[i][j].id], tex[i][j].w);
						idMinWeight[tex[i][j].id] = Min(idMinWeight[tex[i][j].id], tex[i][j].w);
						idCount[tex[i][j].id]++;
					}
				}

				for (UInt32 i = 0; i < 4; ++i)
				{
					for (UInt32 j = 1; j < 3; ++j)
					{
						if (idCount[tex[i][j].id] < 4)
							tex[i][j].w = idMinWeight[tex[i][j].id];
						else
							tex[i][j].w = idMaxWeight[tex[i][j].id];
					}
					EncodeToBuffer(texcoord[i].x_, texcoord[i].y_, tex[i]);
				}

				// ¸´Ô­
				for (UInt32 i = 0; i < 4; ++i)
				{
					for (UInt32 j = 1; j < 3; ++j)
					{
						idMaxWeight[tex[i][j].id] = 0;
						idMinWeight[tex[i][j].id] = 666;
						idCount[tex[i][j].id] = 0;
					}
				}
			}
		}
	}
#endif

	idTexture_->SetData(0, 0, 0, idTexture_->GetWidth(), idTexture_->GetHeight(), idBuffer_.Buffer());
	weightTexture_->SetData(0, 0, 0, weightTexture_->GetWidth(), weightTexture_->GetHeight(), weightBuffer_.Buffer());
}
