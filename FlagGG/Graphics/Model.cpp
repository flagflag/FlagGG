#include "Graphics/Model.h"
#include "Container/Str.h"
#include "Container/ArrayPtr.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		Model::Model(Core::Context* context) :
			Resource(context)
		{ }

		void Model::SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers)
		{
			vertexBuffers_ = vertexBuffers;
		}

		void Model::SetIndexBuffers(const Container::Vector<Container::SharedPtr<IndexBuffer>>& indexBuffers)
		{
			indexBuffers_ = indexBuffers;
		}

		void Model::SetBoneMappings(const Container::Vector<Container::PODVector<uint32_t>>& boneMappings)
		{
			boneMappings_ = boneMappings;
		}

		void Model::SetNumGeometries(uint32_t numGeometries)
		{
			geometries_.Resize(numGeometries);
		}

		void Model::SetNumGeometryLodLevels(uint32_t index, uint32_t num)
		{
			if (index >= geometries_.Size())
				return;

			geometries_[index].Resize(num);
		}

		bool Model::SetGeometry(uint32_t index, uint32_t lodLevel, Geometry* geometry)
		{
			if (index >= geometries_.Size())
				return false;

			if (lodLevel >= geometries_[index].Size())
				return false;

			geometries_[index][lodLevel] = geometry;
			return true;
		}

		const Container::Vector<Container::SharedPtr<VertexBuffer>>& Model::GetVertexBuffers() const
		{
			return vertexBuffers_;
		}

		const Container::Vector<Container::SharedPtr<IndexBuffer>>& Model::GetIndexBuffers() const
		{
			return indexBuffers_;
		}

		const Container::Vector<Container::PODVector<uint32_t>>& Model::GetBoneMappings() const
		{
			return boneMappings_;
		}

		const Scene::Skeleton& Model::GetSkeleton() const
		{
			return skeleton_;
		}

		const Container::Vector <Container::Vector<Container::SharedPtr<Geometry>>>& Model::GetGeometries() const
		{
			return geometries_;
		}

		Geometry* Model::GetGeometry(uint32_t index, uint32_t lodLevel) const
		{
			if (index >= geometries_.Size() || geometries_[index].Empty())
				return nullptr;

			if (lodLevel >= geometries_[index].Size())
				lodLevel = geometries_[index].Size() - 1;

			return geometries_[index][lodLevel];
		}

		uint32_t Model::GetNumGeometries() const
		{
			return geometries_.Size();
		}

		uint32_t Model::GetNumGeometryLodLevels(uint32_t index) const
		{
			if (index >= geometries_.Size())
				return 0u;
			return geometries_[index].Size();
		}

		bool Model::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Container::String fileID;
			fileID.Resize(4);
			stream->ReadStream(&fileID[0], 4);

			if (fileID != "UMDL" && fileID != "UMD2")
			{
				FLAGGG_LOG_ERROR("Invalid model file.");
				return false;
			}

			bool hasVertexDeclarations = (fileID == "UMD2");

			uint32_t numVertexBuffers = 0;
			stream->ReadUInt32(numVertexBuffers);
			uint32_t ignore;
			for (uint32_t i = 0; i < numVertexBuffers; ++i)
			{
				Container::PODVector<VertexElement> vertexElements;
				uint32_t vertexCount = 0;
				stream->ReadUInt32(vertexCount);

				if (!hasVertexDeclarations)
				{
					uint32_t elementMask;
					stream->ReadUInt32(elementMask);
					vertexElements = VertexBuffer::GetElements(elementMask);
				}
				else
				{
					uint32_t numElements = 0;
					stream->ReadUInt32(numElements);
					for (uint32_t j = 0; j < numElements; ++j)
					{
						uint32_t elementDesc;
						stream->ReadUInt32(elementDesc);
						auto type = static_cast<VertexElementType>(elementDesc & 0xffu);
						auto semantic = static_cast<VertexElementSemantic>((elementDesc >> 8u) & 0xffu);
						auto index = static_cast<uint8_t>((elementDesc >> 16u) & 0xffu);
						vertexElements.Push(VertexElement(type, semantic, index));
					}
				}

				stream->ReadUInt32(ignore);
				stream->ReadUInt32(ignore);

				Container::SharedPtr<VertexBuffer> buffer(new VertexBuffer());
				uint32_t vertexSize = VertexBuffer::GetVertexSize(vertexElements);
				buffer->SetSize(vertexCount, vertexElements);
				void* data = buffer->Lock(0, vertexCount);
				stream->ReadStream(data, vertexCount * vertexSize);
				buffer->Unlock();

				vertexBuffers_.Push(buffer);
			}

			uint32_t numIndexBuffers = 0;
			stream->ReadUInt32(numIndexBuffers);
			for (uint32_t i = 0; i < numIndexBuffers; ++i)
			{
				uint32_t indexCount = 0;
				uint32_t indexSize = 0;
				stream->ReadUInt32(indexCount);
				stream->ReadUInt32(indexSize);

				Container::SharedPtr<IndexBuffer> buffer(new IndexBuffer());
				buffer->SetSize(indexSize, indexCount);
				void* data = buffer->Lock(0, indexCount);
				stream->ReadStream(data, indexCount * indexSize);
				buffer->Unlock();

				indexBuffers_.Push(buffer);
			}

			uint32_t numGeometries = 0;
			stream->ReadUInt32(numGeometries);
			boneMappings_.Reserve(numGeometries);
			for (uint32_t i = 0; i < numGeometries; ++i)
			{
				uint32_t boneMappingCount = 0;
				stream->ReadUInt32(boneMappingCount);
				Container::PODVector<unsigned> boneMapping(boneMappingCount);
				for (uint32_t j = 0; j < boneMappingCount; ++j)
				{
					stream->ReadUInt32(boneMapping[j]);
				}
				boneMappings_.Push(boneMapping);

				// lod的图形，远近看到的是不同细节的模型
				uint32_t numLodLevels = 0;
				stream->ReadUInt32(numLodLevels);
				Container::Vector<Container::SharedPtr<Geometry>> lodGeometry;
				lodGeometry.Reserve(numLodLevels);

				for (uint32_t j = 0; j < numLodLevels; ++j)
				{
					float distance = 0.0f;
					uint32_t type = 0;
					uint32_t vertexBufferRef = 0;
					uint32_t indexBufferRef = 0;
					uint32_t indexStart = 0;
					uint32_t indexCount = 0;
					stream->ReadFloat(distance);
					stream->ReadUInt32(type);
					stream->ReadUInt32(vertexBufferRef);
					stream->ReadUInt32(indexBufferRef);
					stream->ReadUInt32(indexStart);
					stream->ReadUInt32(indexCount);

					if (vertexBufferRef > vertexBuffers_.Size())
					{
						FLAGGG_LOG_ERROR("vertex buffer ref out of bounds.");
						return false;
					}
					if (indexBufferRef > indexBuffers_.Size())
					{
						FLAGGG_LOG_ERROR("index buffer ref out of bounds.");
						return false;
					}

					Container::SharedPtr<Geometry> geometry(new Geometry());
					geometry->SetPrimitiveType(static_cast<PrimitiveType>(type));
					geometry->SetVertexBuffer(0, vertexBuffers_[vertexBufferRef]);
					geometry->SetIndexBuffer(indexBuffers_[indexBufferRef]);
					geometry->SetDataRange(indexStart, indexCount);
					geometry->SetLodDistance(distance);

					lodGeometry.Push(geometry);
				}
				geometries_.Push(lodGeometry);
			}

			uint32_t numMorphs = 0;
			stream->ReadUInt32(numMorphs);
			for (uint32_t i = 0; i < numMorphs; ++i)
			{
				Container::String ignoreStr;
				IOFrame::Buffer::ReadString(stream, ignoreStr);
				uint32_t numBuffers = 0;
				stream->ReadUInt32(numBuffers);
				for (uint32_t i = 0; i < numBuffers; ++i)
				{
					stream->ReadUInt32(ignore);
					uint32_t vertexCount = 0;
					uint32_t vertexSize = sizeof(uint32_t);
					stream->ReadUInt32(vertexCount);
					stream->Seek(stream->GetIndex() + vertexCount * vertexSize);
				}
			}

			skeleton_.Load(stream);
			
			return true;
		}

		bool Model::EndLoad()
		{
			return true;
		}
	}
}
