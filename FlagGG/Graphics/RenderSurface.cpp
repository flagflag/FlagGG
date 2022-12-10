#include "Graphics/RenderSurface.h"
#include "Graphics/RenderEngine.h"

namespace FlagGG
{

RenderSurface::RenderSurface(Texture* parentTexture) :
	parentTexture_(parentTexture)
{ }

void RenderSurface::Initialize()
{
}

bool RenderSurface::IsValid()
{
	return GetHandler() != nullptr;
}

Texture* RenderSurface::GetParentTexture()
{
	return parentTexture_;
}

}
