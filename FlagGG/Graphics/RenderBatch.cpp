#include "RenderBatch.h"
#include "Scene/Probe.h"
#include "Graphics/VertexBuffer.h"
#include "GfxDevice/GfxDevice.h"
#include "Memory/Memory.h"

namespace FlagGG
{

RenderBatch::RenderBatch()
{

}

RenderBatch::RenderBatch(const RenderContext& renderContext)
	: geometryType_(renderContext.geometryType_)
	, geometry_(renderContext.geometry_)
	, vertexDesc_(renderContext.vertexDesc_)
	, worldTransform_(renderContext.worldTransform_)
	, numWorldTransform_(renderContext.numWorldTransform_)
	, material_(renderContext.material_)
{

}

RenderInstanceBatchKey::RenderInstanceBatchKey(const RenderBatch& batch)
	: geometry_(batch.geometry_)
	, material_(batch.material_)
	, renderPassInfo_(batch.renderPassInfo_)
	, probe_(batch.probe_)
{

}

UInt32 RenderInstanceBatchKey::ToHash() const
{
	return
		(UInt32)((size_t)geometry_ / sizeof(Geometry) +
			(size_t)renderPassInfo_ / sizeof(RenderPassInfo) +
			(size_t)material_ / sizeof(Material) +
			(size_t)probe_ / sizeof(Probe));
}

RenderInstanceBatch::RenderInstanceBatch(const RenderBatch& rhs)
	: RenderBatch(rhs)
{

}

void RenderBatchQueue::Clear()
{
	renderBatches_.Clear();
	renderInstanceBatches_.Clear();
	instanceBatchMapping_.Clear();
	instanceDataCount_ = 0;
}

void RenderBatchQueue::AddBatch(const RenderBatch& batch, bool allowInstance)
{
	if (allowInstance && GfxDevice::GetDevice()->IsInstanceSupported())
	{
		RenderInstanceBatchKey key(batch);
		auto it = instanceBatchMapping_.Find(batch);
		if (it == instanceBatchMapping_.End())
		{
			it = instanceBatchMapping_.Insert(MakePair(key, renderInstanceBatches_.Size()));
			renderInstanceBatches_.EmplaceBack(batch);
		}

		auto& instBatch = renderInstanceBatches_[it->second_];
		instBatch.instanceCount_++;
		instBatch.worldTransforms_.Push(batch.worldTransform_);
		instanceDataCount_++;
	}
	else
	{
		renderBatches_.EmplaceBack(batch);
	}
}

void RenderBatchQueue::SyncInstanceDataToGpu()
{
	if (renderInstanceBatches_.Empty())
		return;

	if (!instanceBuffer_)
		instanceBuffer_ = new VertexBuffer();

	if (instanceDataCount_ > instanceBuffer_->GetVertexCount() || instanceDataCount_ < instanceBuffer_->GetVertexCount() / 2)
	{
		PODVector<VertexElement> elements =
		{
			VertexElement(VE_VECTOR4, SEM_INSTANCE, 0, true),
			VertexElement(VE_VECTOR4, SEM_INSTANCE, 1, true),
			VertexElement(VE_VECTOR4, SEM_INSTANCE, 2, true),
		};
		instanceBuffer_->SetSize(instanceDataCount_, elements);
	}

	UInt8* dest = reinterpret_cast<UInt8*>(instanceBuffer_->Lock(0, instanceDataCount_));
	UInt32 offsetBytes = 0;
	UInt32 offsetIndex = 0;
	for (auto& renderBatch : renderInstanceBatches_)
	{
		renderBatch.instanceStart_ = offsetIndex;
		for (auto* worldTransform : renderBatch.worldTransforms_)
		{
			Memory::Memcpy(dest + offsetBytes, worldTransform, sizeof(Matrix3x4));
			offsetBytes += sizeof(Matrix3x4);
			++offsetIndex;
		}
	}
	instanceBuffer_->Unlock();
}

}