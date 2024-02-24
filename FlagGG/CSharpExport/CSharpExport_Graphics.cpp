#include "CSharpExport.h"

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Math/Rect.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Log.h"

using namespace FlagGG;

struct DrawTextureArguments
{
	Rect screenRect, sourceRect;
	int leftBorder, rightBorder, topBorder, bottomBorder;
	Color leftBorderColor, rightBorderColor, topBorderColor, bottomBorderColor;
	Color color;
	Vector4 borderWidths;
	Vector4 cornerRadiuses;
	bool smoothCorners;
	int pass;
	Texture* texture;
	Material* mat;
};

CSharp_API void Graphics_DrawTexture(DrawTextureArguments* param)
{

}
