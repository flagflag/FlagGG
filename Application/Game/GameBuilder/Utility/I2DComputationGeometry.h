//
// 2D计算几何函数
//

#pragma once

#include <Math/Vector2.h>

namespace I2DComputationGeometry
{

using namespace FlagGG;

int FloatCompare(float x, float y);

float CrossProduct(const Vector2& v1, const Vector2& v2);

/**
X
/\
|
|
0--------> Y
在此坐标系下，dgree > 0 表示顺时针旋转，dgree < 0 表示逆时针旋转
	*/
Vector2 RotateVector(const Vector2& v, float dgree);

/**
X
/\
|
|
0--------> Y
在此坐标系下，flag=true表示逆时针垂直旋转，false表示顺时针旋垂直转
	*/
Vector2 VerticalRotate(const Vector2& v, bool flag);

bool operator<(const Vector2& v1, const Vector2& v2);

template < typename VectorType >
void PolarAngleSort(VectorType* v, unsigned count)
{
	unsigned best = 0;
	for (unsigned i = 1; i < count; ++i)
	{
		if (v[i] < v[best])
		{
			best = i;
		}
	}
	if (best != 0)
	{
		Swap(v[0], v[best]);
	}

	for (unsigned i = 1; i < count; ++i)
	{
		best = i;
		for (int j = i + 1; j < count; ++j)
		{
			float det = CrossProduct(v[best] - v[0], v[j] - v[0]);
			if (FloatCompare(det, 0.0f) < 0 ||
				(FloatCompare(det, 0.0f) == 0 && FloatCompare((v[best] - v[0]).LengthSquared(), (v[j] - v[0]).LengthSquared()) > 0))
			{
				best = j;
			}
		}

		if (best != i)
		{
			Swap(v[best], v[i]);
		}
	}
}

template < typename VectorType >
void Graham(VectorType* v, unsigned count, unsigned* stack, unsigned& top)
{
	top = -1;
	stack[++top] = 0;
	stack[++top] = 1;
	for (unsigned i = 2; i < count; ++i)
	{
		while (top > 0 && FloatCompare(CrossProduct(v[i] - v[stack[top - 1]], v[stack[top]] - v[stack[top - 1]]), 0.0f) > 0) --top;
		stack[++top] = i;
	}
	++top;
}

bool CheckSegmentCross(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D);

/// 判断点是否在凸多边形内
bool PointInPolygon(const Vector2& P, const Vector2* v, unsigned size);

/// 直线交点
Vector2 LineCrossLine(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D);

/// 点和线段的距离
float PointDistanceToLine(const Vector2& P, const Vector2& A, const Vector2& B);

/// 格式化角度，变成360以内
float FormatAngle(float angle);

}

