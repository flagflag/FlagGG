#include "Scene/StaticMeshComponent.h"
#include "Graphics/RenderEngine.h"

namespace FlagGG
{
	namespace Scene
	{
		void StaticMeshComponent::Update(float timeStep)
		{
		}

		void StaticMeshComponent::Render(Graphics::RenderContext& renderContext)
		{
			renderContext.geometryType_ = GEOMETRY_STATIC;
			renderContext.primitiveType_ = PRIMITIVE_TRIANGLE;
			renderContext.vertexBuffers_ = &model_->GetVertexBuffers();
			renderContext.indexBuffers_ = &model_->GetIndexBuffers();
			renderContext.texture_ = material_->GetTexture();
			renderContext.VSShader_ = material_->GetVSShader();
			renderContext.PSShader_ = material_->GetPSShader();
			renderContext.worldTransform_ = &node_->GetWorldTransform();
			renderContext.numWorldTransform_ = 1;
		}

		void StaticMeshComponent::SetModel(Graphics::Model* model)
		{
			model_ = model;
		}

		void StaticMeshComponent::SetMaterial(Graphics::Material* material)
		{
			material_ = material;
		}
	}
}
