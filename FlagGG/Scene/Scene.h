#pragma once

#include "Export.h"
#include "Core/Context.h"
#include "Scene/Node.h"

namespace FlagGG
{

class Light;

class FlagGG_API Scene : public Node
{
public:
	Scene(Context* context);

	~Scene() override;

	void Start();

	void Stop();

	void Update(const NodeUpdateContext& updateContext) override;

	void Update(Node* node, const NodeUpdateContext& updateContext);

	void Render(PODVector<const RenderContext*>& renderContexts) override;

	void HandleUpdate(Real timeStep);

	void GetLights(PODVector<Light*>& lights);

protected:
	void Render(Node* node, PODVector<const RenderContext*>& renderContexts);

	void GetLights(Node* node, PODVector<Light*>& lights);

	Context* context_;

	bool isRunning_;
};

}
