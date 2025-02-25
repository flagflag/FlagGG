//
// 网格生成工具
//

#pragma once

#include <Container/Ptr.h>
#include <Math/Vector3.h>
#include <Math/Color.h>
#include <Math/BoundingBox.h>
#include <Graphics/GraphicsDef.h>

namespace FlagGG
{

class Model;
class VertexBuffer;
class IndexBuffer;
class Geometry;
struct Bone;

}

using namespace FlagGG;

class MeshBuilder
{
public:
	MeshBuilder();

	/// 创建带刻度的圆圈，外边一层环有描边
	SharedPtr<Model> BuildCircle(const float radius, const float edgeWidth, const Color& color, const float z = 0.f);

	/// 创建环，numScale控制环的角度，numScale=64表示一个圆环
	SharedPtr<Model> BuildArc(const float radius, const float edgeWidth, const int numScale, const Color& color, const float z = 0.f);

	/// 创建扇形，numScale控制扇形的角度，numScale=64表示一个圆
	SharedPtr<Model> BuildSector(const float radius, const int numScale, const Color& color, const float z = 0.f);

	/// 创建一个带游标的扇形
	SharedPtr<Model> BuildSectorWidthVernier(const float radius, const int numScale, const Color& color, const float z = 0.f);

	/// 创建一个用来显示骨骼的模型
	SharedPtr<Model> BuildBoneShowModel(const String& boneName, const Vector<Vector3>& childPositions);

	/// 创建带边宽的正方形
	SharedPtr<Model> BuildSquare(const float width, const float edgeWidth, const Color& color, const float z = 0.f);

	/// 创建带边宽的矩形
	SharedPtr<Model> BuildRect(const float xSize, const float ySize, const float edgeWidth, const Color& color, const float z = 0.f);

	/// 创建一个用来显示弹性骨骼的模型
	SharedPtr<Model> BuildSpringBoneShowModel(const Bone& bone);

protected:
	void Init();

	void Finish(Model* model, const BoundingBox& bbox, PrimitiveType primitiveType = PRIMITIVE_TRIANGLE);

	void AddTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color = Color::WHITE);

	void AddTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& n1, const Vector3& n2, const Vector3& n3, const Color& color = Color::WHITE);

	void AddLine(const Vector3& start, const Vector3& end, float lineWidth, const Color& color);

	void GenerateArc(const float radius, const float edgeWidth, const Color& color, const float z, int numScale);

	void GenerateScale(const float radius, const float edgeWidth, const float z);

	void GenerateTriangleVernier(float radius, int indexScale, const Color& color, float z);

private:
	struct VT
	{
		Vector3 pos;
		Vector3 nor;
		Color color;
	};

	const unsigned numTriangleEachScale = 1;
	PODVector<VT> vts_;
	PODVector<UInt16> indices_;
	SharedPtr<VertexBuffer> vb_;
	SharedPtr<IndexBuffer> ib_;
	SharedPtr<Geometry> geom_;
	BoundingBox bbox_;
};
