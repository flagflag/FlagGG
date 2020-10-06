#include "Graphics/Model.h"
#include "Container/Str.h"
#include "Container/ArrayPtr.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Log.h"
#include "bgfx/bgfx/src/vertexlayout.h"
#include "bgfx/bgfx/3rdparty/meshoptimizer/src/meshoptimizer.h"

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

		void Model::SetBoneMappings(const Container::Vector<Container::PODVector<UInt32>>& boneMappings)
		{
			boneMappings_ = boneMappings;
		}

		void Model::SetNumGeometries(UInt32 numGeometries)
		{
			geometries_.Resize(numGeometries);
		}

		void Model::SetNumGeometryLodLevels(UInt32 index, UInt32 num)
		{
			if (index >= geometries_.Size())
				return;

			geometries_[index].Resize(num);
		}

		bool Model::SetGeometry(UInt32 index, UInt32 lodLevel, Geometry* geometry)
		{
			if (index >= geometries_.Size())
				return false;

			if (lodLevel >= geometries_[index].Size())
				return false;

			geometries_[index][lodLevel] = geometry;
			return true;
		}

		void Model::SetBoundingBox(Math::BoundingBox& box)
		{
			boundingBox_ = box;
		}

		const Container::Vector<Container::SharedPtr<VertexBuffer>>& Model::GetVertexBuffers() const
		{
			return vertexBuffers_;
		}

		const Container::Vector<Container::SharedPtr<IndexBuffer>>& Model::GetIndexBuffers() const
		{
			return indexBuffers_;
		}

		const Container::Vector<Container::PODVector<UInt32>>& Model::GetBoneMappings() const
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

		Geometry* Model::GetGeometry(UInt32 index, UInt32 lodLevel) const
		{
			if (index >= geometries_.Size() || geometries_[index].Empty())
				return nullptr;

			if (lodLevel >= geometries_[index].Size())
				lodLevel = geometries_[index].Size() - 1;

			return geometries_[index][lodLevel];
		}

		UInt32 Model::GetNumGeometries() const
		{
			return geometries_.Size();
		}

		UInt32 Model::GetNumGeometryLodLevels(UInt32 index) const
		{
			if (index >= geometries_.Size())
				return 0u;
			return geometries_[index].Size();
		}

		const Math::BoundingBox& Model::GetBoundingBox() const
		{
			return boundingBox_;
		}

		static void ReadLayout(IOFrame::Buffer::IOBuffer* stream, bgfx::VertexLayout& layout)
		{
			UInt8 numAttrs;
			stream->ReadUInt8(numAttrs);
			UInt16 stride;
			stream->ReadUInt16(stride);

			layout.begin();
			for (UInt32 ii = 0; ii < numAttrs; ++ii)
			{
				UInt16 offset;
				stream->ReadUInt16(offset);

				UInt16 attribId = 0;
				stream->ReadUInt16(attribId);

				UInt8 num;
				stream->ReadUInt8(num);

				UInt16 attribTypeId;
				stream->ReadUInt16(attribTypeId);

				UInt8 normalized;
				stream->ReadUInt8(normalized);

				UInt8 asInt;
				stream->ReadUInt8(asInt);

				bgfx::Attrib::Enum     attr = bgfx::idToAttrib(attribId);
				bgfx::AttribType::Enum type = bgfx::idToAttribType(attribTypeId);
				if (bgfx::Attrib::Count != attr && bgfx::AttribType::Count != type)
				{
					layout.add(attr, num, type, normalized, asInt);
					layout.m_offset[attr] = offset;
				}
			}
			layout.end();
		}

// #define BGFX_MODEL
		bool Model::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
#ifndef BGFX_MODEL
			Container::String fileID;
			fileID.Resize(4);
			stream->ReadStream(&fileID[0], 4);

			if (fileID != "UMDL" && fileID != "UMD2")
			{
				FLAGGG_LOG_ERROR("Invalid model file.");
				return false;
			}

			bool hasVertexDeclarations = (fileID == "UMD2");

			UInt32 numVertexBuffers = 0;
			stream->ReadUInt32(numVertexBuffers);
			UInt32 ignore;
			for (UInt32 i = 0; i < numVertexBuffers; ++i)
			{
				Container::PODVector<VertexElement> vertexElements;
				UInt32 vertexCount = 0;
				stream->ReadUInt32(vertexCount);

				if (!hasVertexDeclarations)
				{
					UInt32 elementMask;
					stream->ReadUInt32(elementMask);
					vertexElements = VertexBuffer::GetElements(elementMask);
				}
				else
				{
					UInt32 numElements = 0;
					stream->ReadUInt32(numElements);
					for (UInt32 j = 0; j < numElements; ++j)
					{
						UInt32 elementDesc;
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
				UInt32 vertexSize = VertexBuffer::GetVertexSize(vertexElements);
				buffer->SetSize(vertexCount, vertexElements);
				void* data = buffer->Lock(0, vertexCount);
				stream->ReadStream(data, vertexCount * vertexSize);
				buffer->Unlock();

				vertexBuffers_.Push(buffer);
			}

			UInt32 numIndexBuffers = 0;
			stream->ReadUInt32(numIndexBuffers);
			for (UInt32 i = 0; i < numIndexBuffers; ++i)
			{
				UInt32 indexCount = 0;
				UInt32 indexSize = 0;
				stream->ReadUInt32(indexCount);
				stream->ReadUInt32(indexSize);

				Container::SharedPtr<IndexBuffer> buffer(new IndexBuffer());
				buffer->SetSize(indexSize, indexCount);
				void* data = buffer->Lock(0, indexCount);
				stream->ReadStream(data, indexCount * indexSize);
				buffer->Unlock();

				indexBuffers_.Push(buffer);
			}

			UInt32 numGeometries = 0;
			stream->ReadUInt32(numGeometries);
			boneMappings_.Reserve(numGeometries);
			for (UInt32 i = 0; i < numGeometries; ++i)
			{
				UInt32 boneMappingCount = 0;
				stream->ReadUInt32(boneMappingCount);
				Container::PODVector<unsigned> boneMapping(boneMappingCount);
				for (UInt32 j = 0; j < boneMappingCount; ++j)
				{
					stream->ReadUInt32(boneMapping[j]);
				}
				boneMappings_.Push(boneMapping);

				// lod的图形，远近看到的是不同细节的模型
				UInt32 numLodLevels = 0;
				stream->ReadUInt32(numLodLevels);
				Container::Vector<Container::SharedPtr<Geometry>> lodGeometry;
				lodGeometry.Reserve(numLodLevels);

				for (UInt32 j = 0; j < numLodLevels; ++j)
				{
					float distance = 0.0f;
					UInt32 type = 0;
					UInt32 vertexBufferRef = 0;
					UInt32 indexBufferRef = 0;
					UInt32 indexStart = 0;
					UInt32 indexCount = 0;
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

			UInt32 numMorphs = 0;
			stream->ReadUInt32(numMorphs);
			for (UInt32 i = 0; i < numMorphs; ++i)
			{
				Container::String ignoreStr;
				IOFrame::Buffer::ReadString(stream, ignoreStr);
				UInt32 numBuffers = 0;
				stream->ReadUInt32(numBuffers);
				for (UInt32 i = 0; i < numBuffers; ++i)
				{
					stream->ReadUInt32(ignore);
					UInt32 vertexCount = 0;
					UInt32 vertexSize = sizeof(UInt32);
					stream->ReadUInt32(vertexCount);
					stream->Seek(stream->GetIndex() + vertexCount * vertexSize);
				}
			}

			skeleton_.Load(stream);

			IOFrame::Buffer::ReadBoundingBox(stream, boundingBox_);
#else
#define G_MAKEFOURCC(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )
#define BGFX_CHUNK_MAGIC_VB  G_MAKEFOURCC('V', 'B', ' ', 0x1)
#define BGFX_CHUNK_MAGIC_VBC G_MAKEFOURCC('V', 'B', 'C', 0x0)
#define BGFX_CHUNK_MAGIC_IB  G_MAKEFOURCC('I', 'B', ' ', 0x0)
#define BGFX_CHUNK_MAGIC_IBC G_MAKEFOURCC('I', 'B', 'C', 0x1)
#define BGFX_CHUNK_MAGIC_PRI G_MAKEFOURCC('P', 'R', 'I', 0x0)
			
			UInt32 chunk;
			Container::SharedPtr<VertexBuffer> vertexBuffer;
			Container::SharedPtr<IndexBuffer> indexBuffer;
			UInt32 fileSize = stream->GetSize();
			while (stream->GetIndex() < fileSize)
			{
				stream->ReadUInt32(chunk);
				switch (chunk)
				{
				case BGFX_CHUNK_MAGIC_VB:
				{
					stream->Seek(stream->GetIndex() + 16);
					stream->Seek(stream->GetIndex() + 24);
					stream->Seek(stream->GetIndex() + 64);

					bgfx::VertexLayout layout;
					ReadLayout(stream, layout);

					UInt16 numVertices;
					stream->ReadUInt16(numVertices);

					vertexBuffer = new VertexBuffer();
					vertexBuffer->SetDynamic(true);
					vertexBuffer->SetSize(numVertices, layout);
					UInt32 dataSize = vertexBuffer->GetVertexSize() * vertexBuffer->GetVertexCount();
					void* data = vertexBuffer->Lock(0, dataSize);
					stream->ReadStream(data, dataSize);
					vertexBuffer->Unlock();

					vertexBuffers_.Push(vertexBuffer);
				}
				break;

				case BGFX_CHUNK_MAGIC_VBC:
				{
					stream->Seek(stream->GetIndex() + 16);
					stream->Seek(stream->GetIndex() + 24);
					stream->Seek(stream->GetIndex() + 64);

					bgfx::VertexLayout layout;
					ReadLayout(stream, layout);

					UInt16 numVertices;
					stream->ReadUInt16(numVertices);

					UInt32 compressedSize;
					stream->ReadUInt32(compressedSize);

					Container::SharedArrayPtr<UInt8> compressedVertices(new UInt8[compressedSize]);
					stream->ReadStream(compressedVertices.Get(), compressedSize);

					vertexBuffer = new VertexBuffer();
					vertexBuffer->SetDynamic(true);
					vertexBuffer->SetSize(numVertices, layout);
					void* data = vertexBuffer->Lock(0, vertexBuffer->GetVertexSize() * vertexBuffer->GetVertexCount());
					meshopt_decodeVertexBuffer(data, vertexBuffer->GetVertexCount(), vertexBuffer->GetVertexSize(), compressedVertices.Get(), compressedSize);
					vertexBuffer->Unlock();

					vertexBuffers_.Push(vertexBuffer);
				}
				break;

				case BGFX_CHUNK_MAGIC_IB:
				{
					UInt32 numIndices;
					stream->ReadUInt32(numIndices);

					indexBuffer = new IndexBuffer();
					indexBuffer->SetDynamic(true);
					indexBuffer->SetSize(2u, numIndices);
					void* data = indexBuffer->Lock(0, 2u * numIndices);
					stream->ReadStream(data, 2u * numIndices);
					indexBuffer->Unlock();

					indexBuffers_.Push(indexBuffer);
				}
				break;

				case BGFX_CHUNK_MAGIC_IBC:
				{
					UInt32 numIndices;
					stream->ReadUInt32(numIndices);

					UInt32 compressedSize;
					stream->ReadUInt32(compressedSize);

					Container::SharedArrayPtr<UInt8> compressedIndices(new UInt8[compressedSize]);
					stream->ReadStream(compressedIndices.Get(), compressedSize);

					indexBuffer = new IndexBuffer();
					indexBuffer->SetDynamic(true);
					indexBuffer->SetSize(2u, numIndices);
					void* data = indexBuffer->Lock(0, 2u * numIndices);
					meshopt_decodeIndexBuffer(data, numIndices, 2, compressedIndices.Get(), compressedSize);
					indexBuffer->Unlock();

					indexBuffers_.Push(indexBuffer);
				}
				break;

				case BGFX_CHUNK_MAGIC_PRI:
				{
					UInt16 len;
					stream->ReadUInt16(len);

					Container::String material;
					material.Resize(len);
					if (len)
						stream->ReadStream(&material[0], len);

					UInt16 num;
					stream->ReadUInt16(num);

					for (UInt32 i = 0; i < num; ++i)
					{
						stream->ReadUInt16(len);

						Container::String name;
						name.Resize(len);
						if (len)
							stream->ReadStream(&name[0], len);

						Container::SharedPtr<Geometry> geometry(new Geometry());
						geometry->SetVertexBuffer(0, vertexBuffer);
						geometry->SetIndexBuffer(indexBuffer);

						UInt32 indexStart, indexCount, vertexStart, vertexCount;
						stream->ReadUInt32(indexStart);
						stream->ReadUInt32(indexCount);
						stream->ReadUInt32(vertexStart);
						stream->ReadUInt32(vertexCount);
						geometry->SetDataRange(indexStart, indexCount, vertexStart, vertexCount);

						Container::Vector<Container::SharedPtr<Geometry>> lodGeometries;
						lodGeometries.Push(geometry);
						geometries_.Push(lodGeometries);

						stream->Seek(stream->GetIndex() + 16);
						stream->Seek(stream->GetIndex() + 24);
						stream->Seek(stream->GetIndex() + 64);
					}
				}
				break;
				}
			}
#endif
			
			return true;
		}

		bool Model::EndLoad()
		{
			return true;
		}

		bool Model::BeginSave(IOFrame::Buffer::IOBuffer* stream)
		{
			Container::String fileID = "UMD2";
			stream->WriteStream(&fileID[0], fileID.Length());

			UInt32 numVertexBuffers = vertexBuffers_.Size();
			stream->WriteUInt32(numVertexBuffers);
			for (UInt32 i = 0; i < numVertexBuffers; ++i)
			{
				auto vertexBuffer = vertexBuffers_[i];
				UInt32 vertexCount = vertexBuffer->GetVertexCount();
				stream->WriteUInt32(vertexCount);
				const auto& vertexElements = vertexBuffer->GetElements();
				UInt32 numElements = vertexElements.Size();
				stream->WriteUInt32(numElements);
				for (auto element : vertexElements)
				{
					UInt32 type = element.vertexElementType_;
					UInt32 semantic = element.vertexElementSemantic_;
					UInt32 index = element.index_;
					UInt32 elementDesc = type | (semantic << 8u) | (index << 16);
					stream->WriteUInt32(elementDesc);
				}

				stream->WriteUInt32(0u);
				stream->WriteUInt32(0u);

				void* data = vertexBuffer->Lock(0, vertexCount);
				stream->WriteStream(data, vertexBuffer->GetVertexSize() * vertexBuffer->GetVertexCount());
				vertexBuffer->Unlock();
			}

			return true;
		}

		bool Model::EndSave()
		{
			return true;
		}
	}
}
