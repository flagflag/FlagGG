#include "Scene/StaticMeshComponent.h"
#include "Graphics/RenderEngine.h"

namespace FlagGG
{
	namespace Scene
	{
		void StaticMeshComponent::Update(float timeStep)
		{
		}

		bool StaticMeshComponent::IsDrawable()
		{
			return true;
		}

		Graphics::RenderContext* StaticMeshComponent::GetRenderContext()
		{
			return &renderContext_;
		}

		void StaticMeshComponent::SetModel(Graphics::Model* model)
		{
			if (model_ == model)
			{
				return;
			}

			model_ = model;

			OnModel();
		}

		void StaticMeshComponent::SetMaterial(Graphics::Material* material)
		{
			material_ = material;

			OnMaterial();
		}

		Graphics::Model* StaticMeshComponent::GetModel()
		{
			return model_;
		}

		Graphics::Material* StaticMeshComponent::GetMaterial()
		{
			return material_;
		}

		void StaticMeshComponent::OnModel()
		{
			renderContext_.geometryType_ = GEOMETRY_STATIC;
			renderContext_.geometries_.Clear();
			//const auto& geometries_ = model_->GetGeometries();
			//for (uint32_t i = 0; i < geometries_.Size(); ++i)
			//{
			//	renderContext_.geometries_.Push(geometries_[i][0]);
			//}
			for (uint32_t i = 0; i < model_->GetNumGeometries(); ++i)
			{
				renderContext_.geometries_.Push(Container::SharedPtr<Graphics::Geometry>(model_->GetGeometry(i, 0)));
			}
			renderContext_.worldTransform_ = &node_->GetWorldTransform();
			renderContext_.numWorldTransform_ = 1;
		}

		void StaticMeshComponent::OnMaterial()
		{
			for (uint32_t i = 0; i < MAX_TEXTURE_CLASS; ++i)
			{
				renderContext_.textures_.Push(material_->GetTexture(i));
			}
			renderContext_.vertexShader_ = material_->GetVertexShader();
			renderContext_.pixelShader_ = material_->GetPixelShader();
			renderContext_.renderPass_ = &material_->GetRenderPass();
			renderContext_.shaderParameters_ = material_->GetShaderParameters();
		}
	}
}
