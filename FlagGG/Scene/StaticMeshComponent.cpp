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
			renderContext.primitiveType_ = PRIMITIVE_TRIANGLE;
			renderContext.vertexBuffers_ = &model_->GetVertexBuffers();
			renderContext.texture_ = material_->GetTexture();
			renderContext.VSShader_ = material_->GetVSShader();
			renderContext.PSShader_ = material_->GetPSShader();
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
