#include "MapBuilder.h"
#include "Map/TerrainTextureWeights.h"
#include "Unit/Terrain.h"

#include <FileSystem/FileSystem.h>
#include <FileSystem/VirtualFileSystem/LocalFileSystem.h>
#include <FileSystem/FileManager.h>
#include <Scene/Scene.h>
#include <Scene/TerrainComponent.h>
#include <Resource/ResourceCache.h>
#include <Resource/Image.h>
#include <Scene/PrefabLoader.h>
#include <Graphics/Material.h>
#include <Graphics/Texture2D.h>
#include <Log.h>

MapBuilder::MapBuilder()
	: tileXCount_(0)
	, tileYCount_(0)
	, terrainBlendTest_(false)
{

}

MapBuilder::~MapBuilder()
{

}

void MapBuilder::SetScene(Scene* scene)
{
	scene_ = scene;
}

void MapBuilder::SetTilePool(TilePool* tilePool)
{

}

void MapBuilder::SetTerrainBlendTest(bool terrainBlendTest)
{
	terrainBlendTest_ = terrainBlendTest;
}

void MapBuilder::LoadMap(const String& path)
{
	auto fileBuffer = GetLocalFileSystem()->OpenFile(path, FileMode::FILE_READ);

	if (!fileBuffer)
	{
		FLAGGG_LOG_INFO("Failed to load path: {}.", path.CString());
		return;
	}

	PODVector<UInt8> tempBuffer;

	Editor::EditorMap::DMapInfo mapInfo;
	Editor::EditorMap::DMapExtraInfo mapExtraInfo;
	Editor::EditorMap::DTileFlagGG tileInfos;
	Editor::EditorMap::DMaterialWeights textureWeights;
	Editor::EditorMap::DStaticDecorationAttribute decoInfos;

	UInt32 subDataHeader = 0;
	while (fileBuffer->ReadStream(&subDataHeader, sizeof(subDataHeader)) == sizeof(subDataHeader))
	{
		UInt32 bufferSize = 0;
		if (fileBuffer->ReadStream(&bufferSize, sizeof(bufferSize)) != sizeof(bufferSize))
		{
			FLAGGG_LOG_INFO("Error map data.");
			return;
		}

		if (bufferSize > 0)
		{
			tempBuffer.Resize(bufferSize);
			if (fileBuffer->ReadStream(&tempBuffer[0], bufferSize) == bufferSize)
			{
				switch (subDataHeader)
				{
				case Editor::EditorMap::DMAPINFO:
				{
					if (!mapInfo.ParseFromArray(tempBuffer.Buffer(), tempBuffer.Size()))
					{
						FLAGGG_LOG_INFO("Failed to parse proto.");
						return;
					}
				}
				break;

				case Editor::EditorMap::DMAPEXTRAINFO:
				{
					if (!mapExtraInfo.ParseFromArray(tempBuffer.Buffer(), tempBuffer.Size()))
					{
						FLAGGG_LOG_INFO("Failed to parse proto.");
						return;
					}
				}
				break;

				case Editor::EditorMap::DTILEFLAGGG:
				{
					if (!tileInfos.ParseFromArray(tempBuffer.Buffer(), tempBuffer.Size()))
					{
						FLAGGG_LOG_INFO("Failed to parse proto.");
						return;
					}
				}
				break;

				case Editor::EditorMap::DMATERIALWEIGHTS:
				{
					if (!textureWeights.ParseFromArray(tempBuffer.Buffer(), tempBuffer.Size()))
					{
						FLAGGG_LOG_INFO("Failed to parse proto.");
						return;
					}
				}
				break;

				case Editor::EditorMap::DSTATICDECORATIONATTRIBUTE:
				{
					if (!decoInfos.ParseFromArray(tempBuffer.Buffer(), tempBuffer.Size()))
					{
						FLAGGG_LOG_INFO("Failed to parse proto.");
						return;
					}
				}
				break;
				}
			}
		}
	}

	tileYCount_ = mapInfo.width();
	tileXCount_ = mapInfo.height();

	GenerateTile(tileInfos);

	GenerateTextureWeights(textureWeights);

	GenerateDecoration(decoInfos);
}

void MapBuilder::GenerateTile(const Editor::EditorMap::DTileFlagGG& tileInfos)
{
	if (terrainBlendTest_)
		return;

	auto* assetFileMgr = GetSubsystem<AssetFileManager>();
	auto* cache = GetSubsystem<ResourceCache>();

	for (Int32 i = 0; i < tileInfos.tile_size(); ++i)
	{
		const auto& tileInfo = tileInfos.tile(i);
		for (Int32 j = 0; j < tileInfo.dataarray_size(); ++j)
		{
			const auto& decoInfo = tileInfo.dataarray(j);

			const String& prefabPath = "tiles/" + String(decoInfo.name().c_str()) + ".prefab";
			auto prefab = LoadPrefab(prefabPath);
			if (prefab)
			{
				prefab->SetPosition(Vector3(decoInfo.loc().x(), decoInfo.loc().y(), decoInfo.loc().z()));
				prefab->SetRotation(Quaternion(decoInfo.quat().w(), decoInfo.quat().x(), decoInfo.quat().y(), decoInfo.quat().z()));
				prefab->SetScale(Vector3(decoInfo.scale().x(), decoInfo.scale().y(), decoInfo.scale().z()));
				scene_->AddChild(prefab);
			}
		}
	}
}

static UInt32 TEX_STYLE_OFFSET[] =
{
	0,
	14,
	26,
	39,
	51,
	61,
	70,
};

void DecodeColor(const Color& color, UInt32& tex1, UInt32& tex2, UInt32& tex3, float& w1, float& w2, float& w3)
{
	tex1 = static_cast<int>(color.r_ * 255) % 16;
	tex2 = (color.r_ * 255 - tex1) / 16;
	tex3 = static_cast<int>(color.g_ * 255) % 16;
	UInt32 texStyle = (color.g_ * 255 - tex3) / 16;
	w1 = color.b_;
	w2 = color.a_;
	w3 = 1.0 - w1 - w2;

	tex1 += TEX_STYLE_OFFSET[texStyle];
	tex2 += TEX_STYLE_OFFSET[texStyle];
	tex3 += TEX_STYLE_OFFSET[texStyle];
}

void MapBuilder::GenerateTextureWeights(const Editor::EditorMap::DMaterialWeights& textureWeights)
{
	UInt32 textureWidth = tileYCount_ * 4 + 1;
	UInt32 textureHeight = tileXCount_ * 4 + 1;

	texWeights_ = new TerrainTextureWeights();
	texWeights_->SetSize(textureWidth, textureHeight);

	for (Int32 i = 0; i < textureWeights.materialweights_size(); ++i)
	{
		const auto& weightsInfo = textureWeights.materialweights(i);
		UInt32 x = weightsInfo.index() / textureHeight;
		UInt32 y = weightsInfo.index() % textureHeight;
		Color color(weightsInfo.r() * 1.f / 255.f, weightsInfo.g() * 1.f / 255.f, weightsInfo.b() * 1.f / 255.f, weightsInfo.a() * 1.f / 255.f);

		TerrainTextureWeights::TextureInfo tex[3];
		DecodeColor(color, tex[0].id, tex[1].id, tex[2].id, tex[0].w, tex[1].w, tex[2].w);

		texWeights_->SetTextureInfo(x, y, tex);
	}

	texWeights_->UpdateGpuTexture();

	if (terrainBlendTest_)
	{
		SharedPtr<Image> heightMap(new Image());
		heightMap->SetSize(textureHeight, textureWidth, 4);
		for (Int32 x = 0; x < heightMap->GetWidth(); ++x)
		{
			for (Int32 y = 0; y < heightMap->GetHeight(); ++y)
			{
				heightMap->SetPixel(x, y, Color::BLACK);
			}
		}
		SharedPtr<Node> terrain(new Node());
		auto* material = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/TerrainLandscape.ljson");
		auto* terrainComp = terrain->CreateComponent<TerrainComponent>();
		terrainComp->SetPatchSize(64);
		terrainComp->SetQuadSize(Vector3(64, 64, 64));
		terrainComp->SetHeightMap(heightMap);
		terrainComp->SetMaterial(material);
		terrainComp->CreateGeometry();
		scene_->AddChild(terrain);
		{
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

void MapBuilder::GenerateDecoration(const Editor::EditorMap::DStaticDecorationAttribute& decoInfos)
{
	for (Int32 i = 0; i < decoInfos.staticdecorationpointattribute_size(); ++i)
	{
		const auto& decoInfo = decoInfos.staticdecorationpointattribute(i);
		const String& prefabPath = decoInfo.fullprefabname().c_str();

		if (!prefabPath.EndsWith(".effect"))
		{
			auto prefab = LoadPrefab(prefabPath);

			prefab->SetPosition(Vector3(decoInfo.position3d().x(), decoInfo.position3d().y(), decoInfo.position3d().z()));
			prefab->SetRotation(Quaternion(decoInfo.quat().w(), decoInfo.quat().x(), decoInfo.quat().y(), decoInfo.quat().z()));
			prefab->SetScale(Vector3(decoInfo.scale().x(), decoInfo.scale().y(), decoInfo.scale().z()));
			scene_->AddChild(prefab);
		}
	}
}
