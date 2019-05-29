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

		Container::Vector<Container::SharedPtr<VertexBuffer>>& Model::GetVertexBuffers()
		{
			return vertexBuffers_;
		}

		const Container::Vector<Container::PODVector<uint32_t>>& Model::GetBoneMappings() const
		{
			return boneMappings_;
		}

		const Scene::Skeleton& Model::GetSkeleton() const
		{
			return skeleton_;
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
				stream->Seek(stream->GetIndex() + indexCount* indexSize);
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

				uint32_t numLodLevels = 0;
				stream->ReadUInt32(numLodLevels);
				for (uint32_t j = 0; j < numLodLevels; ++j)
				{
					stream->Seek(stream->GetIndex() + 6 * sizeof(uint32_t));
				}
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
