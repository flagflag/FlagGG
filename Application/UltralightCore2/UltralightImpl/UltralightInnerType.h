#pragma once

#include "Ultralight/RefPtr.h"

#include <Core/BaseTypes.h>
#include <Math/Vector4.h>

namespace FlagGG
{

class Texture2D;

}

namespace ultralight
{

struct TextureInfo
{
	FlagGG::Texture2D* texture_;
	FlagGG::Vector4 uv_;
};

class RenderTexture : public RefCounted
{
};

}
