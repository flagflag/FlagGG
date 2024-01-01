#include "VertexDescFactory.h"
#include "AsyncFrame/Locker.h"
#include "Math/Math.h"

namespace FlagGG
{

VertexDescription::VertexDescription(UInt32 uuid, const PODVector<VertexElement>& elements)
	: uuid_(uuid)
	, elements_(elements)
{
	strideSize_ = VertexDescFactory::GetVertexSize(elements_);
}

VertexDescription::VertexDescription(UInt32 uuid, PODVector<VertexElement>&& elements)
	: uuid_(uuid)
{
	elements_.Swap(elements);
	strideSize_ = VertexDescFactory::GetVertexSize(elements_);
}

VertexDescFactory::~VertexDescFactory()
{

}

VertexDescription* VertexDescFactory::Create(const PODVector<VertexElement>& elements)
{
	PODVector<VertexElement> formatElements = elements;

	UInt32 offset = 0;
	for (auto& element : formatElements)
	{
		element.offset_ = offset;
		offset += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
	}

	UInt32 uuid = HashVertexElement(formatElements);

	Locker locker(mutex_);

	auto it = vertexDescsMap_.Find(uuid);
	if (it != vertexDescsMap_.End())
	{
		return it->second_;
	}

	VertexDescription* vertexDesc = new VertexDescription(uuid, std::move(formatElements));
	vertexDescsMap_[uuid] = vertexDesc;

	return vertexDesc;
}

void VertexDescFactory::DestroyUnusedVertexDec()
{
	for (auto it = vertexDescsMap_.Begin(); it != vertexDescsMap_.End();)
	{
		if (it->second_->Refs() == 1)
		{
			it = vertexDescsMap_.Erase(it);
		}
		else
		{
			++it;
		}
	}
}

UInt32 VertexDescFactory::HashVertexElement(const PODVector<VertexElement>& elements)
{
	UInt32 hashValue = 0;
	for (const auto& el : elements)
	{
		hashValue = SDBM_Hash(hashValue, ((((el.perInstance_ ? 1 : 0) << 3) | el.vertexElementType_) << 4) | el.vertexElementSemantic_);
		hashValue = SDBM_Hash(hashValue, el.index_);
	}
	return hashValue;
}

PODVector<VertexElement> VertexDescFactory::GetElements(UInt32 elementMask)
{
	PODVector<VertexElement> vertexElements;
	for (UInt32 i = 0; i < MAX_DEFAULT_VERTEX_ELEMENT; ++i)
	{
		if (elementMask & (1u << i))
		{
			vertexElements.Push(DEFAULT_VERTEX_ELEMENT[i]);
		}
	}
	return vertexElements;
}

UInt32 VertexDescFactory::GetVertexSize(const PODVector<VertexElement>& elements)
{
	UInt32 vertexSize = 0u;
	for (const auto& element : elements)
	{
		vertexSize += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
	}
	return vertexSize;
}

}
