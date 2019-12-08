#pragma once

#include "Export.h"
#include "Core/Context.h"
#include "Scene/Node.h"

namespace FlagGG
{
	namespace Scene
	{
		class Light;

		class FlagGG_API Scene : public Node
		{
		public:
			Scene(Core::Context* context);

			~Scene() override;

			void Start();

			void Stop();

			void Update(const NodeUpdateContext& updateContext) override;

			void Update(Node* node, const NodeUpdateContext& updateContext);

			void Render(Container::PODVector<Graphics::RenderContext*>& renderContexts) override;

			void HandleUpdate(float timeStep);

			void GetLights(Container::PODVector<Light*>& lights);

		protected:
			void Render(Node* node, Container::PODVector<Graphics::RenderContext*>& renderContexts);

			void GetLights(Node* node, Container::PODVector<Light*>& lights);

			Core::Context* context_;

			bool isRunning_;
		};
	}
}
