#include "RigidStatic.h"
#include "Scene/StaticMeshComponent.h"

#ifdef FLAGGG_PHYSICS

#endif

namespace FlagGG
{

RigidStatic::RigidStatic()
{
	
}

RigidStatic::~RigidStatic()
{

}

void RigidStatic::OnGlobalAddToNode(Component* component)
{
	if (component && component->GetType() == StaticMeshComponent::GetTypeStatic())
	{
		auto* meshComp = static_cast<StaticMeshComponent*>(component);
		if (auto* model = meshComp->GetModel())
		{
			for (unsigned i = 0; i < model->GetNumGeometries(); ++i)
			{
				auto* geometry = model->GetGeometry(i, 0u);
				auto* vb = geometry->GetVertexBuffer(0);
				auto* ib = geometry->GetIndexBuffer();

#ifdef FLAGGG_PHYSICS

#endif
			}
		}
	}
}

void RigidStatic::OnGlobalRemoveFromNode(Component* component)
{
	if (component == meshComponent_)
	{
#ifdef FLAGGG_PHYSICS

#endif
		meshComponent_ = nullptr;
	}
}

}
