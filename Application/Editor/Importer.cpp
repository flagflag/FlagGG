#include "Importer.h"

// #include <assimp/config.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <Scene/Scene.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/SkeletonMeshComponent.h>
#include <Scene/AnimationComponent.h>
#include <Container/Vector.h>
#include <Container/HashSet.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Quaternion.h>
#include <Graphics/GraphicsDef.h>
#include <IOFrame/Buffer/StringBuffer.h>
#include <IOFrame/Buffer/IOBufferAux.h>
#include <Resource/ResourceCache.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Shader.h>
#include <Log.h>

using namespace FlagGG::Math;
using namespace FlagGG::Graphics;
using namespace FlagGG::IOFrame::Buffer;
using namespace FlagGG::Resource;

namespace Importer
{
	SharedPtr<Context> context_;
	String importPath_;

	static const unsigned MAX_CHANNELS = 4;

	Vector3 ToVector3(const aiVector3D& vec)
	{
		return Vector3(vec.x, vec.y, vec.z);
	}

	Vector2 ToVector2(const aiVector2D& vec)
	{
		return Vector2(vec.x, vec.y);
	}

	Quaternion ToQuaternion(const aiQuaternion& quat)
	{
		return Quaternion(quat.w, quat.x, quat.y, quat.z);
	}

	String FromAIString(const aiString& str)
	{
		return String(str.C_Str());
	}

	static void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale)
	{
		aiVector3D aiPos;
		aiQuaternion aiRot;
		aiVector3D aiScale;
		transform.Decompose(aiScale, aiRot, aiPos);
		pos = ToVector3(aiPos);
		rot = ToQuaternion(aiRot);
		scale = ToVector3(aiScale);
	}

	static PrimitiveType ToPrimitiveType(unsigned primitiveType)
	{
		PrimitiveType type = PRIMITIVE_TRIANGLE;
		switch (primitiveType)
		{
		case aiPrimitiveType_LINE:
			type = PRIMITIVE_LINE;
			break;

		case aiPrimitiveType_TRIANGLE:
			type = PRIMITIVE_TRIANGLE;
			break;
		}
		return type;
	}

	static PODVector<VertexElement> GetVertexElements(aiMesh* aiMsh, bool isSkinned)
	{
		PODVector<VertexElement> ret;

		ret.Push(VertexElement(VE_VECTOR3, SEM_POSITION));

		if (aiMsh->HasNormals())
			ret.Push(VertexElement(VE_VECTOR3, SEM_NORMAL));

		for (uint32_t i = 0; i < aiMsh->GetNumColorChannels() && i < MAX_CHANNELS; ++i)
			ret.Push(VertexElement(VE_UBYTE4_UNORM, SEM_COLOR, i));

		for (uint32_t i = 0; i < aiMsh->GetNumUVChannels() && i < MAX_CHANNELS; ++i)
			ret.Push(VertexElement(VE_VECTOR2, SEM_TEXCOORD, i));

		if (aiMsh->HasTangentsAndBitangents())
			ret.Push(VertexElement(VE_VECTOR4, SEM_TANGENT));

		if (isSkinned)
		{
			ret.Push(VertexElement(VE_VECTOR4, SEM_BLEND_WEIGHTS));
			ret.Push(VertexElement(VE_UBYTE4, SEM_BLEND_INDICES));
		}

		return ret;
	}

	static void WriteVertex(StringBuffer& buffer, aiMesh* aiMsh, bool isSkinned, uint32_t index, 
		const Matrix3x4& vertexTransform, const Matrix3& normalTransform, BoundingBox& boudingBox)
	{
		Vector3 vertex = vertexTransform * ToVector3(aiMsh->mVertices[index]);
		WriteVector3(&buffer, vertex);
		boudingBox.Merge(vertex);

		if (aiMsh->HasNormals())
		{
			Vector3 normal = normalTransform * ToVector3(aiMsh->mNormals[index]);
			WriteVector3(&buffer, normal);
		}

		for (uint32_t i = 0; i < aiMsh->GetNumColorChannels() && i < MAX_CHANNELS; ++i)
		{
			Color color(
				aiMsh->mColors[i][index].r,
				aiMsh->mColors[i][index].g, 
				aiMsh->mColors[i][index].b,
				aiMsh->mColors[i][index].a);
			buffer.WriteUInt32(color.ToUInt());
		}

		for (uint32_t i = 0; i < aiMsh->GetNumUVChannels() && i < MAX_CHANNELS; ++i)
		{
			Vector3 texCoord = ToVector3(aiMsh->mTextureCoords[i][index]);
			buffer.WriteFloat(texCoord.x_);
			buffer.WriteFloat(texCoord.y_);
		}

		if (aiMsh->HasTangentsAndBitangents())
		{
			Vector3 tangent = normalTransform * ToVector3(aiMsh->mTangents[index]);
			Vector3 normal = normalTransform * ToVector3(aiMsh->mNormals[index]);
			Vector3 bitangent = normalTransform * ToVector3(aiMsh->mBitangents[index]);

			float w = 1.0f;
			if (tangent.CrossProduct(normal).DotProduct(bitangent) < 0.5f)
				w = -1.0f;

			WriteVector3(&buffer, tangent);
			buffer.WriteFloat(w);
		}

		if (isSkinned)
		{

		}
	}

	static void WriteIndex(StringBuffer& buffer, aiMesh* aiMsh, uint32_t index, bool largeIndeces)
	{
		if (aiMsh->mFaces[index].mNumIndices != 3)
			return;

		if (!largeIndeces)
		{
			buffer.WriteUInt16(aiMsh->mFaces[index].mIndices[0]);
			buffer.WriteUInt16(aiMsh->mFaces[index].mIndices[1]);
			buffer.WriteUInt16(aiMsh->mFaces[index].mIndices[2]);
		}
		else
		{
			buffer.WriteUInt32(aiMsh->mFaces[index].mIndices[0]);
			buffer.WriteUInt32(aiMsh->mFaces[index].mIndices[1]);
			buffer.WriteUInt32(aiMsh->mFaces[index].mIndices[2]);
		}
	}

	String ToTexturePath(String aiPath)
	{
		auto pos = importPath_.FindLast("/");
		String folder = importPath_.Substring(0, pos);
		aiPath.Replace('\\', '/');
		pos = aiPath.FindLast("/");
		String filename = aiPath.Substring(pos);
		return folder + filename;
	}

	bool HasColor(const PODVector<VertexElement>& elements)
	{
		for (auto el : elements)
		{
			if (el.vertexElementSemantic_ == SEM_COLOR)
				return true;
		}
		return false;
	}

	static HashMap<aiMaterial*, SharedPtr<Material>> materialMap;
	static HashMap<String, SharedPtr<Texture2D>> textureMap;
	static SharedPtr<Node> BuildNode(const aiScene* aiScn, aiNode* aiNd, aiMatrix4x4 transform = {})
	{
		SharedPtr<Node> node(new Node());

		// 设置node的属性
		node->SetName(aiNd->mName.C_Str());
		Vector3 pos;
		Quaternion rot;
		Vector3 scale;
		GetPosRotScale(aiNd->mTransformation, pos, rot, scale);
		node->SetTransform(pos, rot, scale);

		transform = transform * aiNd->mTransformation;
		GetPosRotScale(transform, pos, rot, scale);
		Matrix3x4 vertexTransform = Matrix3x4(pos, rot, scale);
		Matrix3 normalTransform = rot.RotationMatrix();

		if (aiNd->mNumMeshes > 0)
		{
			// 创建Node的Component
			SharedPtr<Model> model(new Model(context_));
			model->SetNumGeometries(aiNd->mNumMeshes);
			Vector<SharedPtr<VertexBuffer>> vertexBuffers;
			Vector<SharedPtr<IndexBuffer>> indexBuffers;
			Vector<SharedPtr<Material>> materialList;
			BoundingBox boundingBox;
			for (uint32_t i = 0; i < aiNd->mNumMeshes; ++i)
			{
				aiMesh* aiMsh = aiScn->mMeshes[aiNd->mMeshes[i]];
				PODVector<VertexElement> vertexElement = GetVertexElements(aiMsh, false);			
				SharedPtr<VertexBuffer> vertexBuffer(new VertexBuffer());
				vertexBuffer->SetSize(aiMsh->mNumVertices, vertexElement);
				uint32_t vertexBufferSize = vertexBuffer->GetVertexSize() * vertexBuffer->GetVertexCount();
				StringBuffer buffer1(vertexBuffer->Lock(0, vertexBufferSize), vertexBufferSize);
				for (uint32_t j = 0; j < aiMsh->mNumVertices; ++j)
				{
					WriteVertex(buffer1, aiMsh, false, j, vertexTransform, normalTransform, boundingBox);
				}
				vertexBuffer->Unlock();

				SharedPtr<IndexBuffer> indexBuffer(new IndexBuffer());
				bool largeIndeces = aiMsh->mNumFaces > sizeof(uint16_t);
				indexBuffer->SetSize(largeIndeces ? sizeof(uint32_t) : sizeof(uint16_t), aiMsh->mNumFaces * 3);
				uint32_t indexBufferSize = indexBuffer->GetIndexSize() * indexBuffer->GetIndexCount();
				StringBuffer buffer2(indexBuffer->Lock(0, indexBufferSize), indexBufferSize);
				for (uint32_t j = 0; j < aiMsh->mNumFaces; ++j)
				{
					WriteIndex(buffer2, aiMsh, j, largeIndeces);
				}
				indexBuffer->Unlock();

				vertexBuffers.Push(vertexBuffer);
				indexBuffers.Push(indexBuffer);

				SharedPtr<Geometry> geometry(new Geometry());
				geometry->SetPrimitiveType(ToPrimitiveType(aiMsh->mPrimitiveTypes));
				geometry->SetVertexBuffer(0, vertexBuffer);
				geometry->SetIndexBuffer(indexBuffer);
				geometry->SetDataRange(0, indexBuffer->GetIndexCount());
				geometry->SetLodDistance(0.0f);
				model->SetNumGeometryLodLevels(i, 1);
				model->SetGeometry(i, 0, geometry);
				
				aiMaterial* aiMtrl = aiScn->mMaterials[aiMsh->mMaterialIndex];
				if (aiMtrl)
				{
					SharedPtr<Material>& material = materialMap[aiMtrl];
					if (!material)
					{
						material = new Material(context_);

						// 获取材质贴图
						aiString stringVal;
						if (aiMtrl->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), stringVal) == AI_SUCCESS)
						{
							String texturePath = FromAIString(stringVal);
							SharedPtr<Texture2D>& texture = textureMap[texturePath];
							if (!texture)
							{
								texture = new Texture2D(context_);
								texture->LoadFile(ToTexturePath(texturePath));
							}
							if (texture->IsValid())
							{
								material->SetTexture(TEXTURE_CLASS_UNIVERSAL, texture);
								material->SetTexture(TEXTURE_CLASS_DIFFUSE, texture);
							}
						}
						if (aiMtrl->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), stringVal) == AI_SUCCESS)
						{
							String texturePath = FromAIString(stringVal);
							SharedPtr<Texture2D>& texture = textureMap[texturePath];
							if (!texture)
							{
								texture = new Texture2D(context_);
								texture->LoadFile(ToTexturePath(texturePath));
							}
							if (texture->IsValid())
							{
								material->SetTexture(TEXTURE_CLASS_NORMAL, texture);
							}
						}
						if (aiMtrl->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), stringVal) == AI_SUCCESS)
						{
							String texturePath = FromAIString(stringVal);
							SharedPtr<Texture2D>& texture = textureMap[texturePath];
							if (!texture)
							{
								texture = new Texture2D(context_);
								texture->LoadFile(ToTexturePath(texturePath));
							}
							if (texture->IsValid())
							{
								material->SetTexture(TEXTURE_CLASS_SPECULAR, texture);
							}
						}
						if (aiMtrl->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), stringVal) == AI_SUCCESS)
						{
							
						}
						if (aiMtrl->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), stringVal) == AI_SUCCESS)
						{
							String texturePath = FromAIString(stringVal);
							SharedPtr<Texture2D>& texture = textureMap[texturePath];
							if (!texture)
							{
								texture = new Texture2D(context_);
								texture->LoadFile(ToTexturePath(texturePath));
							}
							if (texture->IsValid())
							{
								material->SetTexture(TEXTURE_CLASS_EMISSIVE, texture);
							}
						}

						// 获取材质参数
						material->CreateShaderParameters();
						aiColor3D colorVal;
						if (aiMtrl->Get(AI_MATKEY_COLOR_DIFFUSE, colorVal) == AI_SUCCESS)
						{
							Vector4 diffuseColor(colorVal.r, colorVal.g, colorVal.b, 255.0f);
							material->GetShaderParameters()->AddParametersDefine<Vector4>("diffuseColor");
							material->GetShaderParameters()->SetValue("diffuseColor", diffuseColor);
						}
						if (aiMtrl->Get(AI_MATKEY_COLOR_SPECULAR, colorVal) == AI_SUCCESS)
						{
							Vector4 specularColor(colorVal.r, colorVal.g, colorVal.b, 16.0f);
							material->GetShaderParameters()->AddParametersDefine<Vector4>("specularColor");
							material->GetShaderParameters()->SetValue("specularColor", specularColor);
						}
						if (aiMtrl->Get(AI_MATKEY_COLOR_EMISSIVE, colorVal) == AI_SUCCESS)
						{
							Vector4 ambientColor(colorVal.r, colorVal.g, colorVal.b, 16.0f);
							material->GetShaderParameters()->AddParametersDefine<Vector4>("ambientColor");
							material->GetShaderParameters()->SetValue("ambientColor", ambientColor);
						}
						material->GetShaderParameters()->AddParametersDefine<float>("emissivePower");
						material->GetShaderParameters()->SetValue<float>("emissivePower", 2.0f);

						Vector<String> vsdefines = { "STATIC" };
						Vector<String> psdefines = {};
						if (HasColor(vertexElement))
						{
							vsdefines.Push("COLOR");
							psdefines.Push("COLOR");
						}
						ShaderCode* vsShaderCode = context_->GetVariable<ResourceCache>("ResourceCache")->GetResource<ShaderCode>("Shader/Model_VS.hlsl");
						material->SetVertexShader(vsShaderCode->GetShader(VS, vsdefines));
						ShaderCode* psShaderCode = context_->GetVariable<ResourceCache>("ResourceCache")->GetResource<ShaderCode>("Shader/Model_PS.hlsl");
						material->SetPixelShader(psShaderCode->GetShader(PS, psdefines));

						if (!material->GetTexture())
						{
							material->SetTexture(context_->GetVariable<ResourceCache>("ResourceCache")->GetResource<Texture2D>("Textures/White.dds"));
						}
					}
					materialList.Push(material);
				}
			}
			model->SetVertexBuffers(vertexBuffers);
			model->SetIndexBuffers(indexBuffers);
			model->SetBoundingBox(boundingBox);

			SharedPtr<Material> material = materialList.Size()> 0 ?
				materialList[0] :
				context_->GetVariable<ResourceCache>("ResourceCache")->GetResource<Material>("Materials/StaticModel.ljson");

			StaticMeshComponent* meshComp = node->CreateComponent<StaticMeshComponent>();
			meshComp->SetModel(model);
			meshComp->SetMaterial(material);
		}

		for (uint32_t i = 0; i < aiNd->mNumChildren; ++i)
		{
			SharedPtr<Node> child = BuildNode(aiScn, aiNd->mChildren[i], transform);
			node->AddChild(child);
		}

		return node;
	}

	void SetContext(Context* context)
	{
		context_ = context;
	}

	SharedPtr<Node> ImportScene(const String& path)
	{
		importPath_ = path;
		importPath_.Replace('\\', '/');
		unsigned flags = 
			aiProcess_ConvertToLeftHandedZ |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_LimitBoneWeights |
			aiProcess_ImproveCacheLocality |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FixInfacingNormals | 
			aiProcess_FindInvalidData |
			aiProcess_GenUVCoords |
			aiProcess_FindInstances |
			aiProcess_OptimizeMeshes;
		const aiScene* aiScn = aiImportFile(path.CString(), flags);
		if (!aiScn)
			return nullptr;
		materialMap.Clear();
		SharedPtr<Node> root = BuildNode(aiScn, aiScn->mRootNode);
		aiReleaseImport(aiScn);
		return root;
	}

	SharedPtr<Node> ImportModel(const String& path)
	{
		return nullptr;
	}
}

