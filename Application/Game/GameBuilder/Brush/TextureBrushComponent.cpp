#include "TextureBrushComponent.h"
#include "Brush.h"
#include "GameBuilder/Utility/MeshBuilder.h"
#include "Map/TerrainTextureWeights.h"

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
	if (ownerBrush_ && !texWeights_)
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
				
			texWeights_ = new TerrainTextureWeights();
			texWeights_->SetSize(heightMap->GetHeight(), heightMap->GetWidth());

			auto* idTexutre = texWeights_->GetIdTexture();
			auto* weightTexture = texWeights_->GetWeightTexture();

			material->SetTexture((TextureClass)2, idTexutre);
			material->SetTexture((TextureClass)3, weightTexture);

			auto shaderParameters = material->GetShaderParameters();
			shaderParameters->AddParametersDefine<Vector2>("idMapTexels");
			shaderParameters->SetValue<Vector2>("idMapTexels", Vector2(idTexutre->GetWidth(), idTexutre->GetHeight()));
			shaderParameters->AddParametersDefine<Vector2>("weightMapTexels");
			shaderParameters->SetValue<Vector2>("weightMapTexels", Vector2(weightTexture->GetWidth(), weightTexture->GetHeight()));
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

	auto texelsSize = texWeights_->GetSize();

	for (int x = -tsize.x_; x <= tsize.x_; x++)
	{
		for (int y = -tsize.x_; y <= tsize.y_; ++y)
		{
			Vector2 actualPos(x * gridSize.x_ + pos.x_, y * gridSize.y_ + pos.y_);
			IntVector3 dotPos(Round(actualPos.x_), Round(actualPos.y_), 0);
			Vector3 fpos(Round((float)dotPos.x_ / gridSize.x_) * gridSize.x_, Round((float)dotPos.y_ / gridSize.y_) * gridSize.y_, 0);

			if (fpos.x_ < 0 || fpos.y_ < 0)
				continue;

			if (fpos.x_ > texelsSize.x_ || fpos.y_ > texelsSize.y_)
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
				//		nxtPos.x_ < texelsSize.x_ && nxtPos.y_ < texelsSize.y_)
				//	{
				//		UpdateTextureWeight2(nxtPos, 0.0f);
				//	}
				//}
			}
		}
	}

	texWeights_->UpdateGpuTexture();
}

void TextureBrushComponent::UpdateTextureWeight3(const IntVector2& texPos, float blend)
{
	const float brushCenterStrength = 0.2;

	TerrainTextureWeights::TextureInfo tex[3];
	texWeights_->GetTextureInfo(texPos.x_, texPos.y_, tex);

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

	texWeights_->SetTextureInfo(texPos.x_, texPos.y_, tex);
}

void TextureBrushComponent::UpdateTextureWeight2(const IntVector2& texPos, float blend)
{
	const float brushCenterStrength = 0.2;

	TerrainTextureWeights::TextureInfo tex[3];
	texWeights_->GetTextureInfo(texPos.x_, texPos.y_, tex);

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

	texWeights_->SetTextureInfo(texPos.x_, texPos.y_, tex);
}
