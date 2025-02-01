#include "Ultralight/Matrix.h"

#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

void Matrix4x4::SetIdentity()
{
	data[0] = 1.0f;  data[1]  = 0.0f;  data[2]  = 0.0f;  data[3]  = 0.0f;
	data[4] = 0.0f;  data[5]  = 1.0f;  data[6]  = 0.0f;  data[7]  = 0.0f;
	data[8] = 0.0f;  data[9]  = 0.0f;  data[10] = 1.0f;  data[11] = 0.0f;
	data[12] = 0.0f; data[13] = 0.0f; data[14] = 0.0f;   data[15] = 1.0f;
}

void Matrix::SetIdentity()
{
	data[0][0] = 1.0f; data[0][1] = 0.0f; data[0][2] = 0.0f; data[0][3] = 0.0f;
	data[1][0] = 0.0f; data[1][1] = 1.0f; data[1][2] = 0.0f; data[1][3] = 0.0f;
	data[2][0] = 0.0f; data[2][1] = 0.0f; data[2][2] = 1.0f; data[2][3] = 0.0f;
	data[3][0] = 0.0f; data[3][1] = 0.0f; data[3][2] = 0.0f; data[3][3] = 1.0f;
}

void Matrix::SetOrthographicProjection(double screen_width, double screen_height, bool flip_y)
{
	SetIdentity();

	data[0][0] = screen_width;
	data[1][1] = screen_height;
}

void Matrix::Set(const Matrix& other)
{
	Memory::Memcpy(data, other.data, 4 * 4 * sizeof(double));
}

void Matrix::Set(const Matrix4x4& other)
{
	data[0][0] = other.data[0];
	data[0][1] = other.data[1];
	data[0][2] = other.data[2];
	data[0][3] = other.data[3];
	
	data[1][0] = other.data[4];
	data[1][1] = other.data[5];
	data[1][2] = other.data[6];
	data[1][3] = other.data[7];

	data[2][0] = other.data[8];
	data[2][1] = other.data[9];
	data[2][2] = other.data[10];
	data[2][3] = other.data[11];

	data[3][0] = other.data[12];
	data[3][1] = other.data[13];
	data[3][2] = other.data[14];
	data[3][3] = other.data[15];
}

void Matrix::Set(double a, double b, double c, double d, double e, double f)
{
	data[0][0] = a; data[0][1] = b; data[0][2] = 0; data[0][3] = 0;
	data[1][0] = c; data[1][1] = d; data[1][2] = 0; data[1][3] = 0;
	data[2][0] = 0; data[2][1] = 0; data[2][2] = 1; data[2][3] = 0;
	data[3][0] = e; data[3][1] = f; data[3][2] = 0; data[3][3] = 1;
}

void Matrix::Set(double m11, double m12, double m13, double m14,
	double m21, double m22, double m23, double m24,
	double m31, double m32, double m33, double m34,
	double m41, double m42, double m43, double m44)
{
	data[0][0] = m11; data[0][1] = m12; data[0][2] = m13; data[0][3] = m14;
	data[1][0] = m21; data[1][1] = m22; data[1][2] = m23; data[1][3] = m24;
	data[2][0] = m31; data[2][1] = m32; data[2][2] = m33; data[2][3] = m34;
	data[3][0] = m41; data[3][1] = m42; data[3][2] = m43; data[3][3] = m44;
}

bool Matrix::IsIdentity() const
{
	return data[0][0] == 1 && data[0][1] == 0 && data[0][2] == 0 && data[0][3] == 0 &&
		   data[1][0] == 0 && data[1][1] == 1 && data[1][2] == 0 && data[1][3] == 0 &&
		   data[2][0] == 0 && data[2][1] == 0 && data[2][2] == 1 && data[2][3] == 0 &&
		   data[3][0] == 0 && data[3][1] == 0 && data[3][2] == 0 && data[3][3] == 1;
}

bool Matrix::IsIdentityOrTranslation() const
{
	return false;
}

bool Matrix::IsAffine() const
{
	return false;
}

bool Matrix::IsSimple() const
{
	return false;
}

void Matrix::Translate(double x, double y)
{
	data[3][0] += x * data[0][0] + y * data[1][0];
	data[3][1] += x * data[0][1] + y * data[1][1];
	data[3][2] += x * data[0][2] + y * data[1][2];
	data[3][3] += x * data[0][3] + y * data[1][3];
}

void Matrix::Scale(double x, double y)
{
	data[0][0] *= x;
	data[0][1] *= x;
	data[0][2] *= x;
	data[0][3] *= x;

	data[1][0] *= y;
	data[1][1] *= y;
	data[1][2] *= y;
	data[1][3] *= y;
}

static const double piOverFourDouble = 0.785398163397448309616;
static const double piDouble = 3.14159265358979323846;

// Work around a bug in Win, where atan2(+-infinity, +-infinity) yields NaN instead of specific values.
static double wtf_atan2(double x, double y)
{
	double posInf = std::numeric_limits<double>::infinity();
	double negInf = -std::numeric_limits<double>::infinity();
	double nan = std::numeric_limits<double>::quiet_NaN();

	double result = nan;

	if (x == posInf && y == posInf)
		result = piOverFourDouble;
	else if (x == posInf && y == negInf)
		result = 3 * piOverFourDouble;
	else if (x == negInf && y == posInf)
		result = -piOverFourDouble;
	else if (x == negInf && y == negInf)
		result = -3 * piOverFourDouble;
	else
		result = ::atan2(x, y);

	return result;
}

static double rad2deg(double r) { return r * 180.0 / piDouble; }

static double deg2rad(double d) { return d * piDouble / 180.0; }

static void RotateMatrix(Matrix& ret, double rx, double ry, double rz)
{
	// Angles are in degrees. Switch to radians.
	rx = deg2rad(rx);
	ry = deg2rad(ry);
	rz = deg2rad(rz);

	Matrix mat;

	double sinTheta = sin(rz);
	double cosTheta = cos(rz);

	mat.data[0][0] = cosTheta;
	mat.data[0][1] = sinTheta;
	mat.data[0][2] = 0.0;
	mat.data[1][0] = -sinTheta;
	mat.data[1][1] = cosTheta;
	mat.data[1][2] = 0.0;
	mat.data[2][0] = 0.0;
	mat.data[2][1] = 0.0;
	mat.data[2][2] = 1.0;
	mat.data[0][3] = mat.data[1][3] = mat.data[2][3] = 0.0;
	mat.data[3][0] = mat.data[3][1] = mat.data[3][2] = 0.0;
	mat.data[3][3] = 1.0;

	Matrix rmat(mat);

	sinTheta = sin(ry);
	cosTheta = cos(ry);

	mat.data[0][0] = cosTheta;
	mat.data[0][1] = 0.0;
	mat.data[0][2] = -sinTheta;
	mat.data[1][0] = 0.0;
	mat.data[1][1] = 1.0;
	mat.data[1][2] = 0.0;
	mat.data[2][0] = sinTheta;
	mat.data[2][1] = 0.0;
	mat.data[2][2] = cosTheta;
	mat.data[0][3] = mat.data[1][3] = mat.data[2][3] = 0.0;
	mat.data[3][0] = mat.data[3][1] = mat.data[3][2] = 0.0;
	mat.data[3][3] = 1.0;

	rmat.Transform(mat);

	sinTheta = sin(rx);
	cosTheta = cos(rx);

	mat.data[0][0] = 1.0;
	mat.data[0][1] = 0.0;
	mat.data[0][2] = 0.0;
	mat.data[1][0] = 0.0;
	mat.data[1][1] = cosTheta;
	mat.data[1][2] = sinTheta;
	mat.data[2][0] = 0.0;
	mat.data[2][1] = -sinTheta;
	mat.data[2][2] = cosTheta;
	mat.data[0][3] = mat.data[1][3] = mat.data[2][3] = 0.0;
	mat.data[3][0] = mat.data[3][1] = mat.data[3][2] = 0.0;
	mat.data[3][3] = 1.0;

	rmat.Transform(mat);

	ret.Transform(rmat);
}

void Matrix::Rotate(double theta)
{
	RotateMatrix(*this, 0, 0, theta);
}

void Matrix::Rotate(double x, double y)
{
	RotateMatrix(*this, 0, 0, rad2deg(wtf_atan2(y, x)));
}

void Matrix::Transform(const Matrix& other)
{
	Matrix result;
	result.Set(
		data[0][0] * other.data[0][0] + data[0][1] * other.data[1][0] + data[0][2] * other.data[2][0] + data[0][3] * other.data[3][0],
		data[0][0] * other.data[0][1] + data[0][1] * other.data[1][1] + data[0][2] * other.data[2][1] + data[0][3] * other.data[3][1],
		data[0][0] * other.data[0][2] + data[0][1] * other.data[1][2] + data[0][2] * other.data[2][2] + data[0][3] * other.data[3][2],
		data[0][0] * other.data[0][3] + data[0][1] * other.data[1][3] + data[0][2] * other.data[2][3] + data[0][3] * other.data[3][3],
		data[1][0] * other.data[0][0] + data[1][1] * other.data[1][0] + data[1][2] * other.data[2][0] + data[1][3] * other.data[3][0],
		data[1][0] * other.data[0][1] + data[1][1] * other.data[1][1] + data[1][2] * other.data[2][1] + data[1][3] * other.data[3][1],
		data[1][0] * other.data[0][2] + data[1][1] * other.data[1][2] + data[1][2] * other.data[2][2] + data[1][3] * other.data[3][2],
		data[1][0] * other.data[0][3] + data[1][1] * other.data[1][3] + data[1][2] * other.data[2][3] + data[1][3] * other.data[3][3],
		data[2][0] * other.data[0][0] + data[2][1] * other.data[1][0] + data[2][2] * other.data[2][0] + data[2][3] * other.data[3][0],
		data[2][0] * other.data[0][1] + data[2][1] * other.data[1][1] + data[2][2] * other.data[2][1] + data[2][3] * other.data[3][1],
		data[2][0] * other.data[0][2] + data[2][1] * other.data[1][2] + data[2][2] * other.data[2][2] + data[2][3] * other.data[3][2],
		data[2][0] * other.data[0][3] + data[2][1] * other.data[1][3] + data[2][2] * other.data[2][3] + data[2][3] * other.data[3][3],
		data[3][0] * other.data[0][0] + data[3][1] * other.data[1][0] + data[3][2] * other.data[2][0] + data[3][3] * other.data[3][0],
		data[3][0] * other.data[0][1] + data[3][1] * other.data[1][1] + data[3][2] * other.data[2][1] + data[3][3] * other.data[3][1],
		data[3][0] * other.data[0][2] + data[3][1] * other.data[1][2] + data[3][2] * other.data[2][2] + data[3][3] * other.data[3][2],
		data[3][0] * other.data[0][3] + data[3][1] * other.data[1][3] + data[3][2] * other.data[2][3] + data[3][3] * other.data[3][3]
	);

	(*this) = result;
}

bool Matrix::GetInverse(Matrix& result) const
{
	float v0 = data[2][0] * data[3][1] - data[2][1] * data[3][0];
	float v1 = data[2][0] * data[3][2] - data[2][2] * data[3][0];
	float v2 = data[2][0] * data[3][3] - data[2][3] * data[3][0];
	float v3 = data[2][1] * data[3][2] - data[2][2] * data[3][1];
	float v4 = data[2][1] * data[3][3] - data[2][3] * data[3][1];
	float v5 = data[2][2] * data[3][3] - data[2][3] * data[3][2];

	float i00 = (v5 * data[1][1] - v4 * data[1][2] + v3 * data[1][3]);
	float i10 = -(v5 * data[1][0] - v2 * data[1][2] + v1 * data[1][3]);
	float i20 = (v4 * data[1][0] - v2 * data[1][1] + v0 * data[1][3]);
	float i30 = -(v3 * data[1][0] - v1 * data[1][1] + v0 * data[1][2]);

	float invDet = 1.0f / (i00 * data[0][0] + i10 * data[0][1] + i20 * data[0][2] + i30 * data[0][3]);

	i00 *= invDet;
	i10 *= invDet;
	i20 *= invDet;
	i30 *= invDet;

	float i01 = -(v5 * data[0][1] - v4 * data[0][2] + v3 * data[0][3]) * invDet;
	float i11 = (v5 * data[0][0] - v2 * data[0][2] + v1 * data[0][3]) * invDet;
	float i21 = -(v4 * data[0][0] - v2 * data[0][1] + v0 * data[0][3]) * invDet;
	float i31 = (v3 * data[0][0] - v1 * data[0][1] + v0 * data[0][2]) * invDet;

	v0 = data[1][0] * data[3][1] - data[1][1] * data[3][0];
	v1 = data[1][0] * data[3][2] - data[1][2] * data[3][0];
	v2 = data[1][0] * data[3][3] - data[1][3] * data[3][0];
	v3 = data[1][1] * data[3][2] - data[1][2] * data[3][1];
	v4 = data[1][1] * data[3][3] - data[1][3] * data[3][1];
	v5 = data[1][2] * data[3][3] - data[1][3] * data[3][2];

	float i02 = (v5 * data[0][1] - v4 * data[0][2] + v3 * data[0][3]) * invDet;
	float i12 = -(v5 * data[0][0] - v2 * data[0][2] + v1 * data[0][3]) * invDet;
	float i22 = (v4 * data[0][0] - v2 * data[0][1] + v0 * data[0][3]) * invDet;
	float i32 = -(v3 * data[0][0] - v1 * data[0][1] + v0 * data[0][2]) * invDet;

	v0 = data[2][1] * data[1][0] - data[2][0] * data[1][1];
	v1 = data[2][2] * data[1][0] - data[2][0] * data[1][2];
	v2 = data[2][3] * data[1][0] - data[2][0] * data[1][3];
	v3 = data[2][2] * data[1][1] - data[2][1] * data[1][2];
	v4 = data[2][3] * data[1][1] - data[2][1] * data[1][3];
	v5 = data[2][3] * data[1][2] - data[2][2] * data[1][3];

	float i03 = -(v5 * data[0][1] - v4 * data[0][2] + v3 * data[0][3]) * invDet;
	float i13 = (v5 * data[0][0] - v2 * data[0][2] + v1 * data[0][3]) * invDet;
	float i23 = -(v4 * data[0][0] - v2 * data[0][1] + v0 * data[0][3]) * invDet;
	float i33 = (v3 * data[0][0] - v1 * data[0][1] + v0 * data[0][2]) * invDet;

	result.Set(
		i00, i01, i02, i03,
		i10, i11, i12, i13,
		i20, i21, i22, i23,
		i30, i31, i32, i33);

	return true;
}

Point Matrix::Apply(const Point& p) const
{
	return Point();
}

Rect Matrix::Apply(const Rect& r) const
{
	return Rect();
}

uint32_t Matrix::Hash() const
{
	return 0;
}

Matrix4x4 Matrix::GetMatrix4x4() const
{
	Matrix4x4 mat4x4;

	mat4x4.data[0] = data[0][0];
	mat4x4.data[1] = data[0][1];
	mat4x4.data[2] = data[0][2];
	mat4x4.data[3] = data[0][3];

	mat4x4.data[4] = data[1][0];
	mat4x4.data[5] = data[1][1];
	mat4x4.data[6] = data[1][2];
	mat4x4.data[7] = data[1][3];

	mat4x4.data[8]  = data[2][0];
	mat4x4.data[9]  = data[2][1];
	mat4x4.data[10] = data[2][2];
	mat4x4.data[11] = data[2][3];

	mat4x4.data[12] = data[3][0];
	mat4x4.data[13] = data[3][1];
	mat4x4.data[14] = data[3][2];
	mat4x4.data[15] = data[3][3];

	return mat4x4;
}

bool UExport operator==(const Matrix& a, const Matrix& b)
{
	return
		a.data[0][0] == a.data[0][0] &&
		a.data[0][1] == a.data[0][1] &&
		a.data[0][2] == a.data[0][2] &&
		a.data[0][3] == a.data[0][3] &&
		a.data[1][0] == a.data[1][0] &&
		a.data[1][1] == a.data[1][1] &&
		a.data[1][2] == a.data[1][2] &&
		a.data[1][3] == a.data[1][3] &&
		a.data[2][0] == a.data[2][0] &&
		a.data[2][1] == a.data[2][1] &&
		a.data[2][2] == a.data[2][2] &&
		a.data[2][3] == a.data[2][3] &&
		a.data[3][0] == a.data[3][0] &&
		a.data[3][1] == a.data[3][1] &&
		a.data[3][2] == a.data[3][2] &&
		a.data[3][3] == a.data[3][3];
}

bool UExport operator!=(const Matrix& a, const Matrix& b)
{
	return
		a.data[0][0] != a.data[0][0] ||
		a.data[0][1] != a.data[0][1] ||
		a.data[0][2] != a.data[0][2] ||
		a.data[0][3] != a.data[0][3] ||
		a.data[1][0] != a.data[1][0] ||
		a.data[1][1] != a.data[1][1] ||
		a.data[1][2] != a.data[1][2] ||
		a.data[1][3] != a.data[1][3] ||
		a.data[2][0] != a.data[2][0] ||
		a.data[2][1] != a.data[2][1] ||
		a.data[2][2] != a.data[2][2] ||
		a.data[2][3] != a.data[2][3] ||
		a.data[3][0] != a.data[3][0] ||
		a.data[3][1] != a.data[3][1] ||
		a.data[3][2] != a.data[3][2] ||
		a.data[3][3] != a.data[3][3];
}

bool UExport operator==(const Matrix4x4& a, const Matrix4x4& b)
{
	return
		a.data[0] == a.data[0] &&
		a.data[1] == a.data[1] &&
		a.data[2] == a.data[2] &&
		a.data[3] == a.data[3] &&
		a.data[4] == a.data[4] &&
		a.data[5] == a.data[5] &&
		a.data[6] == a.data[6] &&
		a.data[7] == a.data[7] &&
		a.data[8] == a.data[8] &&
		a.data[9] == a.data[9] &&
		a.data[10] == a.data[10] &&
		a.data[11] == a.data[11] &&
		a.data[12] == a.data[12] &&
		a.data[13] == a.data[13] &&
		a.data[14] == a.data[14] &&
		a.data[15] == a.data[15];
}

bool UExport operator!=(const Matrix4x4& a, const Matrix4x4& b)
{
	return
		a.data[0] != a.data[0] ||
		a.data[1] != a.data[1] ||
		a.data[2] != a.data[2] ||
		a.data[3] != a.data[3] ||
		a.data[4] != a.data[4] ||
		a.data[5] != a.data[5] ||
		a.data[6] != a.data[6] ||
		a.data[7] != a.data[7] ||
		a.data[8] != a.data[8] ||
		a.data[9] != a.data[9] ||
		a.data[10] != a.data[10] ||
		a.data[11] != a.data[11] ||
		a.data[12] != a.data[12] ||
		a.data[13] != a.data[13] ||
		a.data[14] != a.data[14] ||
		a.data[15] != a.data[15];
}

}
