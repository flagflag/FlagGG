#include "MeshBuilder.h"
#include "I2DComputationGeometry.h"

#include <Graphics/VertexBuffer.h>
#include <Graphics/IndexBuffer.h>
#include <Graphics/Geometry.h>
#include <Graphics/Model.h>
#include <Scene/Bone.h>

namespace FlagGG
{

static float CrossProduct(const Vector3& v1, const Vector3& v2)
{
	return ::I2DComputationGeometry::CrossProduct(Vector2(v1.x_, v1.y_), Vector2(v2.x_, v2.y_));
}

static bool operator<(const Vector3& v1, const Vector3& v2)
{
	return ::I2DComputationGeometry::FloatCompare(v1.y_, v2.y_) < 0 || (::I2DComputationGeometry::FloatCompare(v1.y_, v2.y_) == 0 && ::I2DComputationGeometry::FloatCompare(v1.x_, v2.x_) < 0);
}

}

MeshBuilder::MeshBuilder()
{
}

void MeshBuilder::Init()
{
	vts_.Clear();
	indices_.Clear();

	vb_ = new VertexBuffer();
	ib_ = new IndexBuffer();
	geom_ = new Geometry();

	bbox_.Clear();
}

void MeshBuilder::Finish(Model* model, const BoundingBox& bbox, PrimitiveType primitiveType/* = PRIMITIVE_TRIANGLE*/)
{
	PODVector<VertexElement> vertexElements;
	vertexElements.Push(VertexElement(VE_VECTOR3, SEM_POSITION));
	vertexElements.Push(VertexElement(VE_VECTOR3, SEM_NORMAL));
	vertexElements.Push(VertexElement(VE_VECTOR4, SEM_COLOR));

	// vb_->SetShadowed(true);
	vb_->SetSize(vts_.Size(), vertexElements);
	vb_->SetData(vts_.Buffer());

	// ib_->SetShadowed(true);
	ib_->SetSize(sizeof(UInt16), indices_.Size());
	ib_->SetData(indices_.Buffer());

	geom_->SetVertexBuffer(0, vb_);
	geom_->SetIndexBuffer(ib_);
	// geom_->SetDrawRange(primitiveType, 0, indices_.Size());
	geom_->SetDataRange(0, indices_.Size());

	Vector<SharedPtr<VertexBuffer>> vertexBuffers;
	Vector<SharedPtr<IndexBuffer>> indexBuffers;
	vertexBuffers.Push(vb_);
	indexBuffers.Push(ib_);

	model->SetNumGeometries(1);
	model->SetGeometry(0, 0, geom_);
	model->SetVertexBuffers(vertexBuffers);
	model->SetIndexBuffers(indexBuffers);
	model->SetBoundingBox(bbox);
}

void MeshBuilder::AddTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color)
{
	Vector3 vec[3] = { v1, v2, v3 };
	I2DComputationGeometry::PolarAngleSort<Vector3>(vec, 3);

	short index = vts_.Size();
	indices_.Push(index++);
	indices_.Push(index++);
	indices_.Push(index++);

	vts_.Push(VT{ vec[0], Vector3(0, 0, 1), color });
	vts_.Push(VT{ vec[1], Vector3(0, 0, 1), color });
	vts_.Push(VT{ vec[2], Vector3(0, 0, 1), color });
}

void MeshBuilder::AddTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& n1, const Vector3& n2, const Vector3& n3, const Color& color)
{
	vts_.Push(VT{ v1, n1, color });
	vts_.Push(VT{ v2, n2, color });
	vts_.Push(VT{ v3, n3, color });
}

void MeshBuilder::AddLine(const Vector3& start, const Vector3& end, float lineWidth, const Color& color)
{
	Vector2 _start(start.x_, start.y_);
	Vector2 _end(end.x_, end.y_);

	// 长度为lineWidth的方向向量
	Vector2 vector = (_end - _start).Normalized() * lineWidth * 0.5;
	// 垂直于vector长度为lineWith的向量，它们的方向相反
	Vector2 rotate1 = I2DComputationGeometry::VerticalRotate(vector, true);
	Vector2 rotate2 = I2DComputationGeometry::VerticalRotate(vector, false);

	// 很明显只需要做个加法就能得出正方形四个顶点的坐标
	Vector3 v1 = Vector3(_start + rotate1, start.z_ + 0.1f);
	Vector3 v2 = Vector3(_start + rotate2, start.z_ + 0.1f);
	Vector3 v3 = Vector3(_end + rotate1, end.z_ + 0.1f);
	Vector3 v4 = Vector3(_end + rotate2, end.z_ + 0.1f);

	AddTriangle(v1, v3, v4, color);
	AddTriangle(v1, v2, v4, color);
}

void MeshBuilder::GenerateArc(const float radius, const float edgeWidth, const Color& color, const float z, int numScale)
{
	if (Abs(numScale) * numTriangleEachScale < 1)
		return;

	const float _360 = 360.0f;
	const unsigned steps = 64 * numTriangleEachScale;
	const int sign = Sign(numScale);
	numScale = Abs(numScale);

	if (edgeWidth == 0.0f)
	{
		for (unsigned i = 1; i <= steps && i <= numScale * numTriangleEachScale; ++i)
		{
			unsigned index = sign > 0 ? i : (steps - i + 1);

			const float angle1 = 1.0f * (index - 1) / steps * _360;
			const float angle2 = 1.0f * index / steps * _360;

			Vector3 v1(radius * Cos(angle1), radius * Sin(angle1), z);
			Vector3 v2(radius * Cos(angle2), radius * Sin(angle2), z);

			AddTriangle(Vector3(0, 0, z), v1, v2, color);

			bbox_.Merge(v1);
			bbox_.Merge(v2);
		}
	}
	else
	{
		for (unsigned i = 1; i <= steps && i <= numScale * numTriangleEachScale; ++i)
		{
			unsigned index = sign > 0 ? i : (steps - i + 1);

			const float angle1 = 1.0f * (index - 1) / steps * _360;
			const float angle2 = 1.0f * index / steps * _360;

			Vector3 v1(radius * Cos(angle1), radius * Sin(angle1), z);
			Vector3 v2(radius * Cos(angle2), radius * Sin(angle2), z);
			Vector3 v3((radius - edgeWidth) * Cos(angle1), (radius - edgeWidth) * Sin(angle1), z);
			Vector3 v4((radius - edgeWidth) * Cos(angle2), (radius - edgeWidth) * Sin(angle2), z);

			AddTriangle(v1, v2, v4, color);
			AddTriangle(v1, v3, v4, color);

			bbox_.Merge(v1);
			bbox_.Merge(v2);
			bbox_.Merge(v3);
			bbox_.Merge(v4);
		}
	}
}

void MeshBuilder::GenerateScale(const float radius, const float edgeWidth, const float z)
{
	const float _360 = 360.0f;
	const float delta = _360 / 64.f;

	for (unsigned i = 0; i < 64; ++i)
	{
		const float angle = delta * i;
		Vector3 v1(radius * Cos(angle), radius * Sin(angle), z);
		if (i % 32 == 0)
		{
			Vector3 v2((radius - edgeWidth) * Cos(angle), (radius - edgeWidth) * Sin(angle), z);
			AddLine(v1, v2, 10, Color::RED);
		}
		else if (i % 16 == 0)
		{
			Vector3 v2((radius - edgeWidth) * Cos(angle), (radius - edgeWidth) * Sin(angle), z);
			AddLine(v1, v2, 10, Color::GREEN);
		}
		else if (i % 4 == 0)
		{
			Vector3 v2((radius - edgeWidth * 0.8f) * Cos(angle), (radius - edgeWidth * 0.8f) * Sin(angle), z);
			AddLine(v1, v2, 2, Color::YELLOW);
		}
		else
		{
			Vector3 v2((radius - edgeWidth * 0.2f) * Cos(angle), (radius - edgeWidth * 0.2f) * Sin(angle), z);
			AddLine(v1, v2, 2, Color::YELLOW);
		}
	}
}

void MeshBuilder::GenerateTriangleVernier(float radius, int indexScale, const Color& color, float z)
{
	const float _360 = 360.0f;
	const unsigned steps = 64 * numTriangleEachScale;

	if (indexScale < 0)
		indexScale = steps - Abs(indexScale);

	const float angle = 1.0f * indexScale * numTriangleEachScale / steps * _360;

	Vector3 v1(radius * Cos(angle), radius * Sin(angle), z);
	Vector2 _v1((radius + 50) * Cos(angle), (radius + 50) * Sin(angle));
	Vector2 vv(_v1.x_ - v1.x_, _v1.y_ - v1.y_);

	Vector3 v2(I2DComputationGeometry::RotateVector(vv, 90) + _v1);
	Vector3 v3(I2DComputationGeometry::RotateVector(vv, -90) + _v1);

	AddTriangle(v1, v2, v3, color);
}

SharedPtr<Model> MeshBuilder::BuildCircle(const float radius, const float edgeWidth, const Color& color, const float z)
{
	Init();

	SharedPtr<Model> model(new Model());

	GenerateArc(radius - edgeWidth, 0.f, Color(color.r_, color.g_, color.b_, color.a_ * 0.5f), z, 64);
	GenerateArc(radius, edgeWidth, color, z, 64);
	GenerateArc(radius + 2, 2, Color(color.r_, color.g_, color.b_, 1), z, 64);
	GenerateArc(radius - edgeWidth, 2, Color(color.r_, color.g_, color.b_, 1), z, 64);

	GenerateScale(radius, edgeWidth, z);
	GenerateTriangleVernier(radius, 0, color, z);

	Finish(model, bbox_);

	return model;
}

SharedPtr<Model> MeshBuilder::BuildArc(const float radius, const float edgeWidth, const int numScale, const Color& color, const float z)
{
	Init();

	SharedPtr<Model> model(new Model());

	GenerateArc(radius, edgeWidth, color, z, numScale);
	GenerateArc(radius + edgeWidth * 0.1, edgeWidth * 0.1, Color(color.r_, color.g_, color.b_, 1), z, numScale);
	GenerateArc(radius - edgeWidth, edgeWidth * 0.1, Color(color.r_, color.g_, color.b_, 1), z, numScale);

	Finish(model, bbox_);

	return model;
}

SharedPtr<Model> MeshBuilder::BuildSector(const float radius, const int numScale, const Color& color, const float z)
{
	Init();

	SharedPtr<Model> model(new Model());

	GenerateArc(radius, 0.f, color, z, numScale);

	Finish(model, bbox_);

	return model;
}

SharedPtr<Model> MeshBuilder::BuildSectorWidthVernier(const float radius, const int numScale, const Color& color, const float z)
{
	Init();

	SharedPtr<Model> model(new Model());

	GenerateArc(radius, 0.f, color, z, numScale);
	GenerateTriangleVernier(radius, numScale, Color::GREEN, z);

	Finish(model, bbox_);

	return model;
}

SharedPtr<Model> MeshBuilder::BuildBoneShowModel(const String& boneName, const Vector<Vector3>& childPositions)
{
	Init();

	SharedPtr<Model> model(new Model());
	model->SetName("bone_show_model_" + boneName);

	bool drawLine = false;
	auto addLine = [&](unsigned si, unsigned ei)
	{
		indices_.Push(si);
		indices_.Push(ei);
	};
	auto addTriangle = [this](unsigned a, unsigned b, unsigned c)
	{
		indices_.Push(a);
		indices_.Push(b);
		indices_.Push(c);
	};

	// 骨骼显示双四棱锥 否则末端骨骼不显示
	float length = 1.0f;
	float width = 1.0f;
	float height = 1.0f;

	Vector3 left(0, -width, 0);
	Vector3 right = -left;
	Vector3 front(length, 0, 0);
	Vector3 back = -front;
	Vector3 top(0, 0, height);
	Vector3 bottom = -top;
	Color color = Color::YELLOW;
	Vector3 normal = Vector3(0, 0, 1);

	bbox_.Merge(left);
	bbox_.Merge(right);
	bbox_.Merge(front);
	bbox_.Merge(back);
	bbox_.Merge(top);
	bbox_.Merge(bottom);
	vts_.Push(VT{ left, normal, color });
	vts_.Push(VT{ right, normal, color });
	vts_.Push(VT{ front, normal, color });
	vts_.Push(VT{ back, normal, color });
	vts_.Push(VT{ top, normal, color });
	vts_.Push(VT{ bottom, normal, color });
	addTriangle(0, 4, 3);
	addTriangle(0, 2, 4);
	addTriangle(0, 5, 2);
	addTriangle(0, 3, 5);
	addTriangle(1, 3, 4);
	addTriangle(1, 4, 2);
	addTriangle(1, 2, 5);
	addTriangle(1, 5, 3);

	// 对每个子骨骼画个四棱锥
	for (unsigned childIdx = 0; childIdx < childPositions.Size(); ++childIdx)
	{
		const Vector3& bonePosition = childPositions[childIdx];
		Vector3 dicZ = bonePosition.Normalized();
		Quaternion rotation(Vector3(0, 0, 1), dicZ);

		float squareSize = Clamp<float>(bonePosition.Length() / 15.0, 0.5, 5);
		Vector3 leftTop = rotation * Vector3(-1, 1, 0) * squareSize;
		Vector3 rightTop = rotation * Vector3(1, 1, 0) * squareSize;
		Vector3 rightBottom = rotation * Vector3(1, -1, 0) * squareSize;
		Vector3 leftBottom = rotation * Vector3(-1, -1, 0) * squareSize;
		Vector3 end = bonePosition;

		bbox_.Merge(leftTop);
		bbox_.Merge(rightTop);
		bbox_.Merge(rightBottom);
		bbox_.Merge(leftBottom);
		bbox_.Merge(end);
		unsigned leftTopIdx = vts_.Size();
		vts_.Push(VT{ leftTop, normal, color });
		unsigned rightTopIdx = vts_.Size();
		vts_.Push(VT{ rightTop, normal, color });
		unsigned rightBottomIdx = vts_.Size();
		vts_.Push(VT{ rightBottom, normal, color });
		unsigned leftBottomIdx = vts_.Size();
		vts_.Push(VT{ leftBottom, normal, color });
		unsigned endIdx = vts_.Size();
		vts_.Push(VT{ end, normal, color });

		addTriangle(endIdx, rightTopIdx, leftTopIdx);
		addTriangle(endIdx, rightBottomIdx, rightTopIdx);
		addTriangle(endIdx, leftBottomIdx, rightBottomIdx);
		addTriangle(endIdx, leftTopIdx, leftBottomIdx);
		addTriangle(rightTopIdx, rightBottomIdx, leftBottomIdx);
		addTriangle(rightTopIdx, leftBottomIdx, leftTopIdx);
	}
	Finish(model, bbox_, PRIMITIVE_TRIANGLE);
	return model;
}

SharedPtr<Model> MeshBuilder::BuildSquare(const float width, const float edgeWidth, const Color& color, const float z)
{
	Init();

	SharedPtr<Model> model(new Model());

	AddTriangle(Vector3(-width * 0.5f, width * 0.5f, z), Vector3(width * 0.5f - edgeWidth, width * 0.5f, z), Vector3(width * 0.5f - edgeWidth, width * 0.5f - edgeWidth, z), Color::RED);
	AddTriangle(Vector3(-width * 0.5f, width * 0.5f, z), Vector3(width * 0.5f - edgeWidth, width * 0.5f - edgeWidth, z), Vector3(-width * 0.5f, width * 0.5f - edgeWidth, z), Color::RED);

	AddTriangle(Vector3(width * 0.5f - edgeWidth, width * 0.5f, z), Vector3(width * 0.5f, width * 0.5f, z), Vector3(width * 0.5f, -width * 0.5f + edgeWidth, z), Color::GREEN);
	AddTriangle(Vector3(width * 0.5f - edgeWidth, width * 0.5f, z), Vector3(width * 0.5f, -width * 0.5f + edgeWidth, z), Vector3(width * 0.5f - edgeWidth, -width * 0.5f + edgeWidth, z), Color::GREEN);

	AddTriangle(Vector3(-width * 0.5f, width * 0.5f - edgeWidth, z), Vector3(-width * 0.5f + edgeWidth, width * 0.5f - edgeWidth, z), Vector3(-width * 0.5f + edgeWidth, -width * 0.5f, z), Color::BLUE);
	AddTriangle(Vector3(-width * 0.5f, width * 0.5f - edgeWidth, z), Vector3(-width * 0.5f + edgeWidth, -width * 0.5f, z), Vector3(-width * 0.5f, -width * 0.5f, z), Color::BLUE);

	AddTriangle(Vector3(-width * 0.5f + edgeWidth, -width * 0.5f + edgeWidth, z), Vector3(width * 0.5f, -width * 0.5f + edgeWidth, z), Vector3(width * 0.5f, -width * 0.5f, z), Color::WHITE);
	AddTriangle(Vector3(-width * 0.5f + edgeWidth, -width * 0.5f + edgeWidth, z), Vector3(width * 0.5f, -width * 0.5f, z), Vector3(-width * 0.5f + edgeWidth, -width * 0.5f, z), Color::WHITE);

	bbox_.Define(Vector3(-width * 0.5f, -width * 0.5f, 0.f), Vector3(width * 0.5f, width * 0.5f, Min(z, 0.1f)));

	Finish(model, bbox_);

	return model;
}

SharedPtr<Model> MeshBuilder::BuildRect(const float xSize, const float ySize, const float edgeWidth, const Color& color, const float z)
{
	Init();

	SharedPtr<Model> model(new Model());

	AddTriangle(Vector3(-xSize * 0.5f, ySize * 0.5f, z), Vector3(xSize * 0.5f - edgeWidth, ySize * 0.5f, z), Vector3(xSize * 0.5f - edgeWidth, ySize * 0.5f - edgeWidth, z), color);
	AddTriangle(Vector3(-xSize * 0.5f, ySize * 0.5f, z), Vector3(xSize * 0.5f - edgeWidth, ySize * 0.5f - edgeWidth, z), Vector3(-xSize * 0.5f, ySize * 0.5f - edgeWidth, z), color);

	AddTriangle(Vector3(xSize * 0.5f - edgeWidth, ySize * 0.5f, z), Vector3(xSize * 0.5f, ySize * 0.5f, z), Vector3(xSize * 0.5f, -ySize * 0.5f + edgeWidth, z), color);
	AddTriangle(Vector3(xSize * 0.5f - edgeWidth, ySize * 0.5f, z), Vector3(xSize * 0.5f, -ySize * 0.5f + edgeWidth, z), Vector3(xSize * 0.5f - edgeWidth, -ySize * 0.5f + edgeWidth, z), color);

	AddTriangle(Vector3(-xSize * 0.5f, ySize * 0.5f - edgeWidth, z), Vector3(-xSize * 0.5f + edgeWidth, ySize * 0.5f - edgeWidth, z), Vector3(-xSize * 0.5f + edgeWidth, -ySize * 0.5f, z), color);
	AddTriangle(Vector3(-xSize * 0.5f, ySize * 0.5f - edgeWidth, z), Vector3(-xSize * 0.5f + edgeWidth, -ySize * 0.5f, z), Vector3(-xSize * 0.5f, -ySize * 0.5f, z), color);

	AddTriangle(Vector3(-xSize * 0.5f + edgeWidth, -ySize * 0.5f + edgeWidth, z), Vector3(xSize * 0.5f, -ySize * 0.5f + edgeWidth, z), Vector3(xSize * 0.5f, -ySize * 0.5f, z), color);
	AddTriangle(Vector3(-xSize * 0.5f + edgeWidth, -ySize * 0.5f + edgeWidth, z), Vector3(xSize * 0.5f, -ySize * 0.5f, z), Vector3(-xSize * 0.5f + edgeWidth, -ySize * 0.5f, z), color);

	bbox_.Define(Vector3(-xSize * 0.5f, -ySize * 0.5f, 0.f), Vector3(xSize * 0.5f, ySize * 0.5f, Min(z, 0.1f)));

	Finish(model, bbox_);

	return model;
}

// 这里显示的骨骼是别人到bone的骨骼
// 按照外侧看去顺时针方向
SharedPtr<Model> MeshBuilder::BuildSpringBoneShowModel(const Bone& bone)
{
	Init();

	SharedPtr<Model> model(new Model());
	model->SetName("springBone_show_model_" + bone.name_);

	Vector3 dicZ = bone.initPosition_.Normalized();
	Quaternion rotation(Vector3(0, 0, 1), dicZ);

	float squareSize = Clamp<float>(bone.initPosition_.Length() / 15.0, 0.5, 5);
	Vector3 leftTop = rotation * Vector3(-1, 1, 0) * squareSize;
	Vector3 rightTop = rotation * Vector3(1, 1, 0) * squareSize;
	Vector3 rightBottom = rotation * Vector3(1, -1, 0) * squareSize;
	Vector3 leftBottom = rotation * Vector3(-1, -1, 0) * squareSize;
	Vector3 endLeftTop = bone.initPosition_ + leftTop;
	Vector3 endRightTop = bone.initPosition_ + rightTop;
	Vector3 endRightBottom = bone.initPosition_ + rightBottom;
	Vector3 endLeftBottom = bone.initPosition_ + leftBottom;

	Color color = Color::YELLOW;
	Vector3 normal = Vector3(0, 0, 1);

	bbox_.Merge(leftTop);			// 0
	bbox_.Merge(rightTop);			// 1
	bbox_.Merge(rightBottom);		// 2
	bbox_.Merge(leftBottom);		// 3
	bbox_.Merge(endLeftTop);		// 4
	bbox_.Merge(endRightTop);		// 5
	bbox_.Merge(endRightBottom);	// 6
	bbox_.Merge(endLeftBottom);		// 7
	vts_.Push(VT{ leftTop, normal, color });
	vts_.Push(VT{ rightTop, normal, color });
	vts_.Push(VT{ rightBottom, normal, color });
	vts_.Push(VT{ leftBottom, normal, color });
	vts_.Push(VT{ endLeftTop, normal, color });
	vts_.Push(VT{ endRightTop, normal, color });
	vts_.Push(VT{ endRightBottom, normal, color });
	vts_.Push(VT{ endLeftBottom, normal, color });

	// 画三角形
	auto addTriangle = [this](unsigned a, unsigned b, unsigned c)
	{
		indices_.Push(a);
		indices_.Push(b);
		indices_.Push(c);
	};

	addTriangle(1, 2, 3);	// 前
	addTriangle(0, 1, 3);
	addTriangle(6, 5, 4);	// 后
	addTriangle(6, 4, 7);
	addTriangle(5, 2, 1);	// 右
	addTriangle(5, 6, 2);
	addTriangle(4, 5, 1);	// 上
	addTriangle(4, 1, 0);
	addTriangle(7, 4, 0);	// 左
	addTriangle(7, 0, 3);
	addTriangle(3, 2, 6);	// 下
	addTriangle(3, 6, 7);

	Finish(model, bbox_, PRIMITIVE_TRIANGLE);

	return model;
}
