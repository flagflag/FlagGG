#include "Graphics/Model.h"
#include "Container/Str.h"
#include "Container/ArrayPtr.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		Model::Model(Core::Context* context) :
			Resource(context),
			batch_(new Batch3D(DRAW_TRIANGLE, nullptr))
		{ }

		Batch3D* Model::GetBatch() const
		{
			return batch_;
		}

		Container::Vector<Container::SharedPtr<VertexBuffer>>& Model::GetVertexBuffers()
		{
			return vertexBuffers_;
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
			for (uint32_t i = 0; i < numVertexBuffers; ++i)
			{
				Container::PODVector<VertexElement> vertexElements;
				uint32_t vertexCount = 0;
				stream->ReadUInt32(vertexCount);

				uint32_t ignore;

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

				//uint32_t vertexSize = 68;
				//Container::SharedArrayPtr<char> buffer(new char[vertexCount * vertexSize]);
				//stream->ReadStream(buffer.Get(), vertexCount * vertexSize);
				//for (uint32_t j = 0; j < vertexCount; ++j)
				//{
				//	batch_->AddBlob(buffer.Get() + j * vertexSize, 12);
				//	batch_->AddBlob(buffer.Get() + j * vertexSize + 12 + 12, 8);
				//	batch_->AddBlob(buffer.Get() + j * vertexSize + 12, 12);
				//}
				Container::SharedPtr<VertexBuffer> buffer(new VertexBuffer());
				uint32_t vertexSize = VertexBuffer::GetVertexSize(vertexElements);
				buffer->SetSize(vertexCount, vertexElements);
				void* data = buffer->Lock(0, vertexCount);
				stream->ReadStream(data, vertexCount * vertexSize);
				buffer->Unlock();

				vertexBuffers_.Push(buffer);
			}
			
			return true;
		}

		bool Model::EndLoad()
		{
			return true;
		}
	}
}
