//
// 地图加载、生成
//

#pragma once

#include <Container/Str.h>
#include <Container/Ptr.h>
#include <Core/Object.h>

#include "Game/Proto/Map.pb.h"

namespace FlagGG
{

class Scene;
class Node;

}

using namespace FlagGG;

class TilePool;
class TerrainTextureWeights;

class MapBuilder : public Object
{
	OBJECT_OVERRIDE(MapBuilder, Object);
public:
	explicit MapBuilder();

	~MapBuilder() override;

	/// 设置关联的场景
	void SetScene(Scene* scene);

	/// 设置瓦片池
	void SetTilePool(TilePool* tilePool);

	/// 加载地图
	void LoadMap(const String& path);

protected:	
	void GenerateTile(const Editor::EditorMap::DTileFlagGG& tileInfos);

	void GenerateTextureWeights(const Editor::EditorMap::DMaterialWeights& textureWeights);

	void GenerateDecoration(const Editor::EditorMap::DStaticDecorationAttribute& decoInfos);

private:
	// 场景
	SharedPtr<Scene> scene_;

	// 瓦片节点
	SharedPtr<Node> tileNode_;

	// 装饰物节点
	SharedPtr<Node> decoNode_;

	// 地表纹理权重
	SharedPtr<TerrainTextureWeights> texWeights_;

	UInt32 tileXCount_;
	UInt32 tileYCount_;
};
