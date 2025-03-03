#include "TextureBrushComponent.h"
#include "Brush.h"
#include "GameBuilder/Utility/MeshBuilder.h"

#include <Core/DeviceEvent.h>
#include <Scene/TerrainComponent.h>
#include <Scene/TerrainPatchComponent.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>
#include <Scene/Octree.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Material.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include <TypeTraits/IsArray.h>

#define BLEND_TEST 0
#define SPLAT_MAP3 1

TextureBrushComponent::TextureBrushComponent()
	: isWorking_(false)
	, brushSize_(2.56)
	, brushId_(1)
{

}

TextureBrushComponent::~TextureBrushComponent()
{

}

void TextureBrushComponent::OnAttach()
{

}

void TextureBrushComponent::OnDetach()
{

}

void TextureBrushComponent::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	if (mouseKey == MOUSE_LEFT)
	{
		isWorking_ = true;

		lastFrameMousePos_ = IntVector2::ZERO;
	}
}

void TextureBrushComponent::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	isWorking_ = false;
}

void TextureBrushComponent::OnMouseMove(KeyState* keyState, const Vector2& delta)
{

}

void TextureBrushComponent::OnWheel()
{

}

void TextureBrushComponent::OnKeyUp(KeyState* keyState, UInt32 keyCode)
{
	if (keyCode == VK_F6)
	{
		brushId_ = Min(brushId_ + 1, 80);
	}

	if (keyCode == VK_F7)
	{
		brushId_ = Max(brushId_ - 1, 1);
	}
}

void TextureBrushComponent::OnUpdate(float timeStep)
{
	TryCreateData();

	auto* cameraNode = ownerBrush_->GetOwnerScene()->GetChild(String("MainCamera"), true);
	auto* camera = cameraNode->GetComponent<Camera>();
	auto* octree = ownerBrush_->GetOwnerScene()->GetComponentRecursive<Octree>();

	IntRect rect = GetDesktopRect();
	auto mousePos = GetSubsystem<Input>()->GetMousePos();

	Ray ray = camera->GetScreenRay((float)mousePos.x_ / rect.Width(), (float)mousePos.y_ / rect.Height());
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, ray, RAY_QUERY_AABB, F_INFINITY, DRAWABLE_TERRAIN);
	octree->Raycast(query);

	for (auto& ret : results)
	{
		if (ret.component_->IsInstanceOf<TerrainPatchComponent>())
		{
			hudCircle_->SetPosition(ret.position_);

			if (isWorking_ && mousePos != lastFrameMousePos_)
			{
				UpdateTextureWeight(ret);

				lastFrameMousePos_ = mousePos;
			}
		}
	}
}

void TextureBrushComponent::SetBrushId(UInt32 brushId)
{
	brushId_ = brushId;
}

void TextureBrushComponent::TryCreateData()
{
	if (ownerBrush_ && (!idImage_ || !weightImage_))
	{
		MeshBuilder meshBuilder;
		auto mesh = meshBuilder.BuildArc(brushSize_, 0.02, 64, Color::GREEN, 0.005);
		auto material = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/3DHud.ljson");

		hudCircle_ = new Node();
		auto* meshComp = hudCircle_->CreateComponent<StaticMeshComponent>();
		meshComp->SetModel(mesh);
		meshComp->SetMaterial(material);

		ownerBrush_->GetOwnerScene()->AddChild(hudCircle_);

		auto* terrainComp = ownerBrush_->GetOwnerScene()->GetComponentRecursive<TerrainComponent>();
		if (terrainComp)
		{
			auto* material = terrainComp->GetMaterial();
			auto* heightMap = terrainComp->GetHeightMap();

			idImage_ = new Image();
			idImage_->SetSize(heightMap->GetHeight(), heightMap->GetWidth(), 4);
			for (Int32 x = 0; x < idImage_->GetWidth(); ++x)
			{
				for (Int32 y = 0; y < idImage_->GetHeight(); ++y)
				{
					idImage_->SetPixel(x, y, Color(2.0 / 255, 255, 2.0 / 255.0));
				}
			}

			weightImage_ = new Image();
			weightImage_->SetSize(heightMap->GetHeight(), heightMap->GetWidth(), 4);
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
			idTexture_->SetSize(heightMap->GetHeight() * 3, heightMap->GetWidth() * 3, TEXTURE_FORMAT_RGBA8);
#else
			idTexture_->SetSize(heightMap->GetHeight(), heightMap->GetWidth(), TEXTURE_FORMAT_RGBA8);
#endif

			weightTexture_ = new Texture2D();
			weightTexture_->SetNumLevels(1);
			weightTexture_->SetFilterMode(TEXTURE_FILTER_BILINEAR);
			weightTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
			weightTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
			weightTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
#if SPLAT_MAP3
			weightTexture_->SetSize(heightMap->GetHeight() * 3, heightMap->GetWidth() * 3, TEXTURE_FORMAT_RGBA8);
#else
			weightTexture_->SetSize(heightMap->GetHeight() * 2, heightMap->GetWidth() * 2, TEXTURE_FORMAT_RGBA8);
#endif

			UpdateGpuTexture();

			material->SetTexture((TextureClass)2, idTexture_);
			material->SetTexture((TextureClass)3, weightTexture_);

			auto shaderParameters = material->GetShaderParameters();
			shaderParameters->AddParametersDefine<Vector2>("idMapTexels");
			shaderParameters->SetValue<Vector2>("idMapTexels", Vector2(idTexture_->GetWidth(), idTexture_->GetHeight()));
			shaderParameters->AddParametersDefine<Vector2>("weightMapTexels");
			shaderParameters->SetValue<Vector2>("weightMapTexels", Vector2(weightTexture_->GetWidth(), weightTexture_->GetHeight()));
		}
	}
}

void TextureBrushComponent::UpdateTextureWeight(const RayQueryResult& result)
{
	const IntVector2 gridSize(1, 1);
	const float sizeSquared = brushSize_ * brushSize_;
	IntVector2 tsize(Round(brushSize_ / gridSize.x_), Round(brushSize_ / gridSize.y_));

	Vector3 pos(
		Round(result.position_.x_ / gridSize.x_) * gridSize.x_,
		Round(result.position_.y_ / gridSize.y_) * gridSize.y_,
		result.position_.z_);

	Vector2 realPos(result.position_.x_, result.position_.y_);

	IntVector2 direction[] =
	{
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ -1, 1 },
		{ -1, 0 },
		{ -1, -1 },
		{ 0, -1 },
		{ 1, -1 },
	};

	for (int x = -tsize.x_; x <= tsize.x_; x++)
	{
		for (int y = -tsize.x_; y <= tsize.y_; ++y)
		{
			Vector2 actualPos(x * gridSize.x_ + pos.x_, y * gridSize.y_ + pos.y_);
			IntVector3 dotPos(Round(actualPos.x_), Round(actualPos.y_), 0);
			Vector3 fpos(Round((float)dotPos.x_ / gridSize.x_) * gridSize.x_, Round((float)dotPos.y_ / gridSize.y_) * gridSize.y_, 0);

			if (fpos.x_ < 0 || fpos.y_ < 0)
				continue;

			if (fpos.x_ > idImage_->GetWidth() || fpos.y_ > idImage_->GetHeight())
				continue;

			float lengthSquared = Vector2(fpos.x_ - realPos.x_, fpos.y_ - realPos.y_).LengthSquared();
			if (lengthSquared <= sizeSquared)
			{
				float distPercent = Min(Sqrt(lengthSquared) / brushSize_, (float)1.0);
				float alpha = 1.0 - distPercent * distPercent;

				IntVector2 gridPos(fpos.x_ / gridSize.x_, fpos.y_ / gridSize.y_);
				float blend = Equals(alpha, 0.f) ? 0.f : alpha;

				// UpdateTextureWeight3(gridPos, blend);
				UpdateTextureWeight2(gridPos, blend);

				//for (Int32 k = 0; k < 8; ++k)
				//{
				//	IntVector2 nxtPos = gridPos + direction[k];
				//	if (nxtPos.x_ >= 0 && nxtPos.y_ >= 0 &&
				//		nxtPos.x_ < idImage_->GetWidth() && nxtPos.y_ < idImage_->GetHeight())
				//	{
				//		UpdateTextureWeight2(nxtPos, 0.0f);
				//	}
				//}
			}
		}
	}

	UpdateGpuTexture();
}

void TextureBrushComponent::UpdateTextureWeight3(const IntVector2& texPos, float blend)
{
	const float brushCenterStrength = 0.2;

	TextureInfo tex[3];
	DecodeFromImage(texPos.x_, texPos.y_, tex);

	int findIdx = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (tex[i].id == brushId_)
		{
			findIdx = i;
			break;
		}
	}

	float finalBlendFactor = blend * brushCenterStrength;

	if (tex[findIdx].id == brushId_)
	{
		finalBlendFactor = tex[findIdx].w + powf(powf(finalBlendFactor, 1.f + brushCenterStrength - finalBlendFactor), fabs(finalBlendFactor - tex[findIdx].w) + (1.f - brushCenterStrength));
	}

#if BLEND_TEST
	finalBlendFactor = 1.0f;
#else
	finalBlendFactor = Clamp(finalBlendFactor, 0.01f, 1.f);
#endif

	int otherIdx1 = (findIdx + 1) % 3;
	int otherIdx2 = (findIdx + 2) % 3;

	tex[findIdx].id = brushId_;
	tex[findIdx].w = finalBlendFactor;

	if (Equals(tex[otherIdx1].w + tex[otherIdx2].w, 0.f))
	{
		tex[otherIdx1].w = 0.f;
		tex[otherIdx2].w = 0.f;
		tex[findIdx].w = 1.f;
	}
	else
	{
		float leftSum = 1.0 - tex[findIdx].w;
		float otherSum = tex[otherIdx1].w + tex[otherIdx2].w;
		float scale = leftSum / otherSum;
		tex[otherIdx1].w *= scale;
		tex[otherIdx2].w *= scale;
	}

	std::sort(tex, tex + 3, [](const decltype(tex[0])& _1, const decltype(tex[0])& _2)
	{
		return _1.w < _2.w;
	});

	EncodeToImage(texPos.x_, texPos.y_, tex);
}

void TextureBrushComponent::UpdateTextureWeight2(const IntVector2& texPos, float blend)
{
	const float brushCenterStrength = 0.2;

	TextureInfo tex[3];
	DecodeFromImage(texPos.x_, texPos.y_, tex);

	int findIdx = 1;
	for (int i = 1; i < 3; ++i)
	{
		if (tex[i].id == brushId_)
		{
			findIdx = i;
			break;
		}
	}

	float finalBlendFactor = blend * brushCenterStrength;

	if (tex[findIdx].id == brushId_)
	{
		finalBlendFactor = tex[findIdx].w + powf(powf(finalBlendFactor, 1.f + brushCenterStrength - finalBlendFactor), fabs(finalBlendFactor - tex[findIdx].w) + (1.f - brushCenterStrength));
	}

#if BLEND_TEST
	finalBlendFactor = 1.0f;
#else
	finalBlendFactor = Clamp(finalBlendFactor, 0.01f, 1.f);
#endif

	int otherIdx = findIdx == 1 ? 2 : 1;

	tex[findIdx].id = brushId_;
	tex[findIdx].w = finalBlendFactor;
	tex[otherIdx].w = 1.0 - finalBlendFactor;
	
	if (tex[1].w > tex[2].w)
	{
		Swap(tex[1], tex[2]);
	}

	EncodeToImage(texPos.x_, texPos.y_, tex);
}

void TextureBrushComponent::DecodeFromImage(UInt32 x, UInt32 y, TextureInfo* texInfo)
{
	const Color color = idImage_->GetPixel(x, y);
	const Color weight = weightImage_->GetPixel(x, y);

	texInfo[0] = { FloorToInt(color.r_ * 255.0 + 0.5), weight.r_ };
	texInfo[1] = { FloorToInt(color.g_ * 255.0 + 0.5), weight.g_ };
	texInfo[2] = { FloorToInt(color.b_ * 255.0 + 0.5), weight.b_ };
}

void TextureBrushComponent::EncodeToImage(UInt32 x, UInt32 y, const TextureInfo* texInfo)
{
	idImage_->SetPixel(x, y, Color(texInfo[0].id / 255.0, texInfo[1].id / 255.0, texInfo[2].id / 255.0));
	weightImage_->SetPixel(x, y, Color(texInfo[0].w, texInfo[1].w, texInfo[2].w));
}

void TextureBrushComponent::EncodeToBuffer(UInt32 x, UInt32 y, const TextureInfo* texInfo)
{
	idBuffer_[y * idTexture_->GetWidth() + x] = Color(texInfo[0].id / 255.0, texInfo[1].id / 255.0, texInfo[2].id / 255.0).ToUInt();
	weightBuffer_[y * weightTexture_->GetWidth() + x] = Color(texInfo[0].w, texInfo[1].w, texInfo[2].w).ToUInt();
}

void TextureBrushComponent::EncodeIdToBuffer(UInt32 x, UInt32 y, UInt32 id0, UInt32 id1, UInt32 id2)
{
	idBuffer_[y * idTexture_->GetWidth() + x] = Color(id0 / 255.0, id1 / 255.0, id2 / 255.0).ToUInt();
}

void TextureBrushComponent::EncodeWeightToBuffer(UInt32 x, UInt32 y, float weight0, float weight1, float weight2)
{
	weightBuffer_[y * weightTexture_->GetWidth() + x] = Color(weight0, weight1, weight2).ToUInt();
}

void TextureBrushComponent::DecodeFromBuffer(UInt32 x, UInt32 y, TextureInfo* texInfo)
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

void TextureBrushComponent::UpdateGpuTexture()
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

				// 复原
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
