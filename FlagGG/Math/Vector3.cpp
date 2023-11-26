#include "Math/Vector3.h"
#include "Math/Math.h"

namespace FlagGG
{

const Vector3 Vector3::ZERO;
const Vector3 Vector3::LEFT(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::RIGHT(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UP(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::DOWN(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::FORWARD(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::BACK(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

const IntVector3 IntVector3::ZERO;
const IntVector3 IntVector3::LEFT(-1, 0, 0);
const IntVector3 IntVector3::RIGHT(1, 0, 0);
const IntVector3 IntVector3::UP(0, 1, 0);
const IntVector3 IntVector3::DOWN(0, -1, 0);
const IntVector3 IntVector3::FORWARD(0, 0, 1);
const IntVector3 IntVector3::BACK(0, 0, -1);
const IntVector3 IntVector3::ONE(1, 1, 1);

String Vector3::ToString(unsigned decimals) const
{
	char tempBuffer[CONVERSION_BUFFER_LENGTH];
	sprintf(tempBuffer, "%.*f %.*f %.*f", decimals, x_, decimals, y_, decimals, z_);
	return String(tempBuffer);
}

String Vector3::ToString() const
{
    char tempBuffer[CONVERSION_BUFFER_LENGTH];
    sprintf(tempBuffer, "%.2f %.2f %.2f", x_, y_, z_);
    return String(tempBuffer);
}

Vector3 Vector3::FromString(const String& str)
{
	auto vec = str.Split(' ');
	if (vec.Size() == 3)
		return Vector3(std::atof(vec[0].CString()), std::atof(vec[1].CString()), std::atof(vec[2].CString()));
	return
		Vector3::ZERO;
}

String IntVector3::ToString() const
{
    char tempBuffer[CONVERSION_BUFFER_LENGTH];
    sprintf(tempBuffer, "%d %d %d", x_, y_, z_);
    return String(tempBuffer);
}

}
