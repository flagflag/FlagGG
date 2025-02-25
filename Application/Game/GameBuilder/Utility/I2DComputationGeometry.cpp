#include "I2DComputationGeometry.h"

namespace I2DComputationGeometry
{

// 精度误差
static const float eps = 1e-5;

int FloatCompare(float x, float y)
{
	if (fabs(x - y) < eps) return 0;
	return x - y > 0.0f ? 1 : -1;
}

float CrossProduct(const Vector2& v1, const Vector2& v2)
{
	return v1.x_ * v2.y_ - v1.y_ * v2.x_;
}

Vector2 RotateVector(const Vector2& v, float dgree)
{
	return Vector2(v.x_ * Cos(dgree) - v.y_ * Sin(dgree), v.x_ * Sin(dgree) + v.y_ * Cos(dgree));
}

Vector2 VerticalRotate(const Vector2& v, bool flag)
{
	return Vector2(v.y_ * (flag ? 1 : -1), v.x_ * (flag ? -1 : 1));
}

bool operator<(const Vector2& v1, const Vector2& v2)
{
	return FloatCompare(v1.y_, v2.y_) < 0 || (FloatCompare(v1.y_, v2.y_) == 0 && FloatCompare(v1.x_, v2.x_) < 0);
}

/// 两线段相交
/// 要注意两个特殊情况：1.线平行，这种不能算相交；2.交点在线的端点，这种情况一个端点会被计算两次
bool CheckSegmentCross(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D)
{
	if (FloatCompare(CrossProduct(A - B, C - D), 0.0f) == 0) // 判断平行
	{
		return false;
	}

	// A、B两点在线段CD两侧
	if (FloatCompare(CrossProduct(C - A, D - A) * CrossProduct(C - B, D - B), 0.0f) <= 0)
	{
		// A、B两点
		if (FloatCompare(CrossProduct(B - A, C - A) * CrossProduct(B - A, D - A), 0.0f) <= 0)
		{
			return true;
		}
	}

	return false;
}

/// 判断点是否在凸多边形内
bool PointInPolygon(const Vector2& P, const Vector2* v, unsigned size)
{
	Vector2 AuxiliaryPoint(P.x_, 1e5);
	unsigned crossCount = 0;

	for (unsigned i = 0; i < size; ++i)
	{
		if (CheckSegmentCross(P, AuxiliaryPoint, v[i], v[(i + 1) % size]) &&
			FloatCompare(CrossProduct(AuxiliaryPoint - P, v[(i + 1) % size] - P), 0.0f) != 0) // 防止线的端点被算进去两次
		{
			++crossCount;
		}
	}

	return crossCount % 2;
}

Vector2 LineCrossLine(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D)
{
	float u = CrossProduct(B - A, C - A);
	float v = CrossProduct(A - B, D - B);
	return Vector2((C.x_ * v + D.x_ * u) / (u + v), (C.y_ * v + D.y_ * u) / (u + v));
}

float PointDistanceToLine(const Vector2& P, const Vector2& A, const Vector2& B)
{
	Vector2 projectP = P.ProjectOntoLine(A, B);
	return P.DistanceToPoint(projectP);
}

float FormatAngle(float angle)
{
	if (angle > 0.0f)
	{
		return angle - 360.0f * ((int)angle / 360);
	}
	else
	{
		return angle + 360.0f * (((int)angle + 360 - 1) / 360);
	}
}

}

