#include "Graphics/Model.h"
#include "Container/Str.h"
#include "Container/ArrayPtr.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Core/ObjectFactory.h"
#include "Log.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(Model);

Model::Model() :
	Resource()
{ }

void Model::SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& vertexBuffers)
{
	vertexBuffers_ = vertexBuffers;
}

void Model::SetIndexBuffers(const Vector<SharedPtr<IndexBuffer>>& indexBuffers)
{
	indexBuffers_ = indexBuffers;
}

void Model::SetBoneMappings(const Vector<PODVector<UInt32>>& boneMappings)
{
	boneMappings_ = boneMappings;
}

void Model::SetNumGeometries(UInt32 numGeometries)
{
	geometries_.Resize(numGeometries);

	// For easier creation of from-scratch geometry, ensure that all geometries start with at least 1 LOD level (0 makes no sense)
	for (unsigned i = 0; i < geometries_.Size(); ++i)
	{
		if (geometries_[i].Empty())
			geometries_[i].Resize(1);
	}
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

void Model::SetBoundingBox(const BoundingBox& box)
{
	boundingBox_ = box;
}

const Vector<SharedPtr<VertexBuffer>>& Model::GetVertexBuffers() const
{
	return vertexBuffers_;
}

const Vector<SharedPtr<IndexBuffer>>& Model::GetIndexBuffers() const
{
	return indexBuffers_;
}

const Vector<PODVector<UInt32>>& Model::GetBoneMappings() const
{
	return boneMappings_;
}

const Skeleton& Model::GetSkeleton() const
{
	return skeleton_;
}

const Vector <Vector<SharedPtr<Geometry>>>& Model::GetGeometries() const
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

const BoundingBox& Model::GetBoundingBox() const
{
	return boundingBox_;
}

bool Model::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String fileID;
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
		PODVector<VertexElement> vertexElements;
		UInt32 vertexCount = 0;
		stream->ReadUInt32(vertexCount);

		if (!hasVertexDeclarations)
		{
			UInt32 elementMask;
			stream->ReadUInt32(elementMask);
			vertexElements = VertexDescFactory::GetElements(elementMask);
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

		SharedPtr<VertexBuffer> buffer(new VertexBuffer());
		UInt32 vertexSize = VertexDescFactory::GetVertexSize(vertexElements);
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

		SharedPtr<IndexBuffer> buffer(new IndexBuffer());
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
		PODVector<unsigned> boneMapping(boneMappingCount);
		for (UInt32 j = 0; j < boneMappingCount; ++j)
		{
			stream->ReadUInt32(boneMapping[j]);
		}
		boneMappings_.Push(boneMapping);

		// lod的图形，远近看到的是不同细节的模型
		UInt32 numLodLevels = 0;
		stream->ReadUInt32(numLodLevels);
		Vector<SharedPtr<Geometry>> lodGeometry;
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

			SharedPtr<Geometry> geometry(new Geometry());
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
		String ignoreStr;
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
			
	return true;
}

bool Model::EndLoad()
{
	return true;
}

bool Model::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
	String fileID = "UMD2";
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
