#include "Scene/StaticMeshComponent.h"
#include "Graphics/RenderEngine.h"

namespace FlagGG
{

void StaticMeshComponent::Update(Real timeStep)
{
}

bool StaticMeshComponent::IsDrawable()
{
	return true;
}

RenderContext* StaticMeshComponent::GetRenderContext()
{
	return &renderContext_;
}

void StaticMeshComponent::OnUpdateWorldBoundingBox()
{
	worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
}

void StaticMeshComponent::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
	switch (query.level_)
	{
	case RAY_QUERY_AABB:
		Component::ProcessRayQuery(query, results);
		break;

	case RAY_QUERY_OBB:

		break;
	}
}

void StaticMeshComponent::SetViewMask(UInt32 viewMask)
{
	Component::SetViewMask(viewMask);

	renderContext_.viewMask_ = GetViewMask();
}

void StaticMeshComponent::SetModel(Model* model)
{
	if (model_ == model)
	{
		return;
	}

	model_ = model;

	OnModel();
}

void StaticMeshComponent::SetMaterial(Material* material)
{
	material_ = material;

	OnMaterial();
}

Model* StaticMeshComponent::GetModel()
{
	return model_;
}

Material* StaticMeshComponent::GetMaterial()
{
	return material_;
}

void StaticMeshComponent::OnModel()
{
	renderContext_.geometryType_ = GEOMETRY_STATIC;
	renderContext_.geometries_.Clear();
	//const auto& geometries_ = model_->GetGeometries();
	//for (UInt32 i = 0; i < geometries_.Size(); ++i)
	//{
	//	renderContext_.geometries_.Push(geometries_[i][0]);
	//}
	for (UInt32 i = 0; i < model_->GetNumGeometries(); ++i)
	{
		renderContext_.geometries_.Push(SharedPtr<Geometry>(model_->GetGeometry(i, 0)));
	}
	renderContext_.worldTransform_ = &node_->GetWorldTransform();
	renderContext_.numWorldTransform_ = 1;
	renderContext_.viewMask_ = GetViewMask();

	boundingBox_ = model_->GetBoundingBox();
}

void StaticMeshComponent::OnMaterial()
{
	renderContext_.textures_.Clear();
	for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
	{
		renderContext_.textures_.Push(material_->GetTexture(i));
	}
	renderContext_.vertexShader_ = material_->GetVertexShader();
	renderContext_.pixelShader_ = material_->GetPixelShader();
	renderContext_.renderPass_ = &material_->GetRenderPass();
	renderContext_.shaderParameters_ = material_->GetShaderParameters();
	renderContext_.rasterizerState_ = material_->GetRasterizerState();
}

}
