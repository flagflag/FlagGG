#include "AmbientProbeBaker.h"
#include "Graphics/GraphicsDef.h"
#include "Resource/Image.h"

namespace FlagGG
{

// Keep in sync with SphericalHarmonicsL2::kCoeffCount and Wintermute::ProbeBakeTechnique
#define SH_COEFF_COUNT  9

// 1 / (2*sqrt(kPI))
#define K1DIV2SQRTPI        0.28209479177387814347403972578039f
// sqrt(3) / (2*sqrt(kPI))
#define KSQRT3DIV2SQRTPI    0.48860251190291992158638462283835f
// sqrt(15) / (2*sqrt(kPI))
#define KSQRT15DIV2SQRTPI   1.0925484305920790705433857058027f
// 3 * sqrtf(5) / (4*sqrt(kPI))
#define K3SQRT5DIV4SQRTPI   0.94617469575756001809268107088713f
// sqrt(15) / (4*sqrt(kPI))
#define KSQRT15DIV4SQRTPI   0.54627421529603953527169285290135f
// sqrt(5)/(4*sqrt(kPI)) - comes from the missing -1 in K3SQRT5DIV4SQRTPI when compared to appendix A2 in http://www.ppsloan.org/publications/StupidSH36.pdf
#define KALMOSTONETHIRD     0.315391565252520050f
// 16*kPI/17
#define KNORMALIZATION      2.9567930857315701067858823529412f

static const Vector3 kCubemapOrthoBases[6 * 3] =
{
	Vector3(0, 0, -1), Vector3(0, -1, 0), Vector3(-1, 0, 0),
	Vector3(0, 0, 1), Vector3(0, -1, 0), Vector3(1, 0, 0),
	Vector3(1, 0, 0), Vector3(0, 0, 1), Vector3(0, -1, 0),
	Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0),
	Vector3(1, 0, 0), Vector3(0, -1, 0), Vector3(0, 0, -1),
	Vector3(-1, 0, 0), Vector3(0, -1, 0), Vector3(0, 0, 1),
};

static void SHEvalDirection9(const Vector3 dir, float* const outsh)
{
	outsh[0] = K1DIV2SQRTPI;
	outsh[1] = -dir.y_ * KSQRT3DIV2SQRTPI;
	outsh[2] = dir.z_ * KSQRT3DIV2SQRTPI;
	outsh[3] = -dir.x_ * KSQRT3DIV2SQRTPI;
	outsh[4] = dir.x_ * dir.y_ * KSQRT15DIV2SQRTPI;
	outsh[5] = -dir.y_ * dir.z_ * KSQRT15DIV2SQRTPI;
	outsh[6] = (dir.z_ * dir.z_ * K3SQRT5DIV4SQRTPI) + (-KALMOSTONETHIRD);
	outsh[7] = -dir.x_ * dir.z_ * KSQRT15DIV2SQRTPI;
	outsh[8] = (dir.x_ * dir.x_ - dir.y_ * dir.y_) * KSQRT15DIV4SQRTPI;
}

static void ProjectSkyboxFaceIntoSH(Image* face, const Vector3& basisX, const Vector3& basisY,
	const Vector3& basisZ, bool inputIsSRGB,
	SphericalHarmonicsL2& outAmbientProbe)
{
	SphericalHarmonicsL2 tempProbe;
	tempProbe.SetZero();
	Vector4 shtmp[SphericalHarmonicsL2::kCoeffCountSIMD];

	const int size = face->GetWidth();

	// We'll need pixel center coordinates in -1..1 space, so
	// basically (-1 + 1/size) to (1 - 1/size) when integer goes from
	// 0 to size-1.
	const float coordBias = -1.f + 1.f / size;
	const float coordScale = 2.f / size;

	float weightSum = 0.0f;

	// go over all pixels of a cubemap face
	for (int y = 0; y < size; ++y)
	{
		float fy = y * coordScale + coordBias;
		for (int x = 0; x < size; ++x)
		{
			float fx = x * coordScale + coordBias;

			// fx, fy are pixel coordinates in -1..+1 range
			float ftmp = 1.0f + fx * fx + fy * fy;
			float linearWeight = 4.0f / (Sqrt(ftmp) * ftmp);

			// evaluate SH in pixel's direction and weight by solid angle
			Vector3 dir = (basisZ + basisX * fx + basisY * fy).Normalized();
			SHEvalDirection9(dir, (float*)shtmp);

			// convert data from pixel values to floats in Linear colorspace
			Color rgbaf = face->GetPixel(x, y);

			if (inputIsSRGB)
			{
				rgbaf[0] = GammaToLinearSpace(rgbaf[0]);
				rgbaf[1] = GammaToLinearSpace(rgbaf[1]);
				rgbaf[2] = GammaToLinearSpace(rgbaf[2]);
			}

			// accumulate into overall SH
			rgbaf[0] *= linearWeight;
			rgbaf[1] *= linearWeight;
			rgbaf[2] *= linearWeight;

			tempProbe.AddToCoefficients(shtmp, rgbaf.Data());

			weightSum += linearWeight;
		}
	}

	// normalize
	for (int i = 0; i < SphericalHarmonicsL2::kCoeffCount; ++i)
		tempProbe.GetCoefficient(SphericalHarmonics::kColorChannelRed, i) *= SphericalHarmonicsL2::kNormalizationConstants[i];
	for (int i = 0; i < SphericalHarmonicsL2::kCoeffCount; ++i)
		tempProbe.GetCoefficient(SphericalHarmonics::kColorChannelGreen, i) *= SphericalHarmonicsL2::kNormalizationConstants[i];
	for (int i = 0; i < SphericalHarmonicsL2::kCoeffCount; ++i)
		tempProbe.GetCoefficient(SphericalHarmonics::kColorChannelBlue, i) *= SphericalHarmonicsL2::kNormalizationConstants[i];

	// normalize SH and add to output. Division by 6 since we are only building one cubemap face here.
	const float normWeight = 4.0f * PI / weightSum / 6.0f;
	outAmbientProbe.AddWeighted(tempProbe, normWeight);
}

bool BakeAmbientProbe(Image* cubemap, SphericalHarmonicsL2& outAmbientProbe)
{
	outAmbientProbe.SetZero();

	auto* cubemapFace = cubemap;

	for (unsigned i = 0; i < 6; ++i)
	{
		if (!cubemapFace)
			return false;

		const Vector3& basisX = kCubemapOrthoBases[i * 3 + 0];
		const Vector3& basisY = kCubemapOrthoBases[i * 3 + 1];
		const Vector3& basisZ = -kCubemapOrthoBases[i * 3 + 2];

		ProjectSkyboxFaceIntoSH(cubemapFace, basisX, basisY, basisZ, false, outAmbientProbe);

		cubemapFace = cubemapFace->GetNextSibling();
	}

	return true;
}

/*
SHAr = vec4Coeffs[0];
SHAg = vec4Coeffs[1];
SHAb = vec4Coeffs[2];

SHBr = vec4Coeffs[3];
SHBg = vec4Coeffs[4];
SHBb = vec4Coeffs[5];

SHC = vec4Coeffs[6];

PBR/PBRCommon.glsl 
	SHEvalLinearL0L1(normal) + SHEvalLinearL2(normal)
*/
Color SampleProbe(const SphericalHarmonicsL2::Vec4Coeffs& vec4Coeffs, const Vector4& normal)
{
	Color ambientColor;
	// L0L1
	{
		ambientColor.r_ = vec4Coeffs[0].DotProduct(normal);
		ambientColor.g_ = vec4Coeffs[1].DotProduct(normal);
		ambientColor.b_ = vec4Coeffs[2].DotProduct(normal);
	}
	// L2
	{
		Color x1, x2;
		// 4 of the quadratic (L2) polynomials
		Vector4 vB = Vector4(normal.x_, normal.y_, normal.z_, normal.z_) * Vector4(normal.y_, normal.z_, normal.z_, normal.x_);
		x1.r_ = vec4Coeffs[3].DotProduct(vB);
		x1.g_ = vec4Coeffs[4].DotProduct(vB);
		x1.b_ = vec4Coeffs[5].DotProduct(vB);

		// Final (5th) quadratic (L2) polynomial
		float vC = normal.x_ * normal.x_ - normal.y_ * normal.y_;
		x2 = Color(vec4Coeffs[6].x_ * vC, vec4Coeffs[6].y_ * vC, vec4Coeffs[6].z_ * vC);

		ambientColor += (x1 + x2);
	}

	ambientColor.a_ = 1.f;

	return ambientColor;
}

}
