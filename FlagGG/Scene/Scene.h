#pragma once

#include "Export.h"
#include "Core/Context.h"
#include "Scene/Node.h"
#include "Container/HashSet.h"

namespace FlagGG
{

class DrawableComponent;
class Octree;
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

	void GetLights(PODVector<Light*>& lights);

	void OnAddToScene(Node* node, Component* component);

	void OnRemoveFromScene(Node* node, Component* component);

protected:
	void HandleUpdate(Real timeStep);

	void HandlePreRenderUpdate(Real timeStep);

	void Render(Node* node, PODVector<const RenderContext*>& renderContexts);

	void GetLights(Node* node, PODVector<Light*>& lights);

	Context* context_;

	bool isRunning_;

	Octree* octree_;

	HashSet<DrawableComponent*> addToOctreeSet_;
};

}
