#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Graphics/RenderContext.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API Component : public Container::RefCounted
		{
		public:
			virtual void Update(float timeStep) = 0;

			virtual void Render(Graphics::RenderContext& renderContext) = 0;

		private:

		};
	}
}
