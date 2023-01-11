#include "VertexDescFactory.h"
#include "AsyncFrame/Locker.h"
#include "Math/Math.h"

namespace FlagGG
{

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

}
