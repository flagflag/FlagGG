#include "Graphics/RenderSurface.h"
#include "Graphics/RenderEngine.h"

namespace FlagGG
{
	namespace Graphics
	{
		void RenderSurface::Initialize()
		{
		}

		bool RenderSurface::IsValid()
		{
			return GetHandler() != nullptr;
		}
	}
}
