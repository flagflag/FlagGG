#pragma once

#include "Export.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API Scene : public Node
		{
		public:
			void Update(float timeStep) override;

			void Render(Container::Vector<Graphics::RenderContext>& renderContexts) override;

		protected:
			void Render(Node* node, Container::Vector<Graphics::RenderContext>& renderContexts);
		};
	}
}
