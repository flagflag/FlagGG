#pragma once

#include "Export.h"
#include "Core/Context.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API Scene : public Node
		{
		public:
			Scene(Core::Context* context);

			~Scene() override;

			void Start();

			void Stop();

			void Update(float timeStep) override;

			void Update(Node* node, float timeStep);

			void Render(Container::PODVector<Graphics::RenderContext*>& renderContexts) override;

			void HandleUpdate(float timeStep);

		protected:
			void Render(Node* node, Container::PODVector<Graphics::RenderContext*>& renderContexts);

			Core::Context* context_;

			bool isRunning_;
		};
	}
}
