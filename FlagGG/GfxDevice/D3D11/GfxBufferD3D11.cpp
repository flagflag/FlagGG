#include "GfxBufferD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "Log.h"
#include "Core/CryAssert.h"

namespace FlagGG
{

static D3D11_USAGE d3d11Usage[]=
{
	D3D11_USAGE_DEFAULT,
	D3D11_USAGE_DYNAMIC
};

static D3D11_MAP d3d11Map[] =
{
	D3D11_MAP_WRITE,
	D3D11_MAP_WRITE_DISCARD
};

GfxBufferD3D11::GfxBufferD3D11()
	: GfxBuffer()
{

}

GfxBufferD3D11::~GfxBufferD3D11()
{
	D3D11_SAFE_RELEASE(d3d11Buffer_);
}

void GfxBufferD3D11::Apply(const void* initialDataPtr)
{
	if (d3d11Buffer_)
	{
		D3D11_SAFE_RELEASE(d3d11Buffer_);
	}

	UINT bindFlags = 0;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_VERTEX)
		bindFlags |= D3D11_BIND_VERTEX_BUFFER;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_INDEX)
		bindFlags |= D3D11_BIND_INDEX_BUFFER;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_BIND_UNIFORM)
		bindFlags |= D3D11_BIND_CONSTANT_BUFFER;

	UINT accessFlags = 0;
	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE)
		accessFlags |= D3D11_CPU_ACCESS_WRITE;
	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_READ)
		accessFlags |= D3D11_CPU_ACCESS_READ;

	UINT byteCount = gfxBufferDesc_.size_;
	if (byteCount % 16u != 0)
		byteCount += (16u - (byteCount % 16u));

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = accessFlags;
	desc.Usage = d3d11Usage[gfxBufferDesc_.usage_];
	desc.ByteWidth = byteCount;

	HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11Device()->CreateBuffer(&desc, nullptr, &d3d11Buffer_);
	if (FAILED(hr))
	{
		D3D11_SAFE_RELEASE(d3d11Buffer_);
		FLAGGG_LOG_ERROR("Failed to create vertex buffer.");
		return;
	}
}

void GfxBufferD3D11::UpdateBuffer(const void* dataPtr)
{
	// 静态buffer不修改数据
	if (gfxBufferDesc_.usage_ == BUFFER_USAGE_STATIC)
	{
		CRY_ASSERT(false);
		return;
	}

	void* dstDataPtr = BeginWrite(0, gfxBufferDesc_.size_);
	if (dstDataPtr)
	{
		memcpy(dstDataPtr, dataPtr, gfxBufferDesc_.size_);
		EndWrite(gfxBufferDesc_.size_);
	}
}

void GfxBufferD3D11::UpdateBufferRange(const void* dataPtr, UInt32 offset, UInt32 size)
{
	// 静态buffer不修改数据
	if (gfxBufferDesc_.usage_ == BUFFER_USAGE_STATIC)
	{
		CRY_ASSERT(false);
		return;
	}

	void* dstDataPtr = BeginWrite(offset, size);
	if (dstDataPtr)
	{
		memcpy(static_cast<char*>(dstDataPtr) + offset, dataPtr, size);
		EndWrite(size);
	}
}

void* GfxBufferD3D11::BeginWrite(UInt32 offset, UInt32 size)
{
	if (!d3d11Buffer_)
	{
		CRY_ASSERT(false);
		return nullptr;
	}

	if (offset + size > gfxBufferDesc_.size_)
	{
		CRY_ASSERT(false);
		return nullptr;
	}

	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		memset(&mappedData, 0, sizeof mappedData);
		HRESULT hr = GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->Map(d3d11Buffer_, 0, d3d11Map[gfxBufferDesc_.usage_], 0, &mappedData);
		if (FAILED(hr) || !mappedData.pData)
		{
			FLAGGG_LOG_ERROR("Failed to Map buffer data.");
			return nullptr;
		}
		return static_cast<char*>(mappedData.pData) + offset;
	}
	else
	{
		shadowdData_.Resize(size);
		return &shadowdData_[0];
	}
}

void GfxBufferD3D11::EndWrite(UInt32 bytesWritten)
{
	if (!d3d11Buffer_)
	{
		CRY_ASSERT(false);
		return;
	}

	if (gfxBufferDesc_.accessFlags_ & BUFFER_ACCESS_WRITE)
	{
		GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->Unmap(d3d11Buffer_, 0);
	}
	else
	{
		D3D11_BOX destBox;
		destBox.left = 0;
		destBox.right = Min(bytesWritten, shadowdData_.Size());
		destBox.top = 0;
		destBox.bottom = 1;
		destBox.front = 0;
		destBox.back = 1;

		GetSubsystem<GfxDeviceD3D11>()->GetD3D11DeviceContext()->UpdateSubresource(d3d11Buffer_, 0, &destBox, &shadowdData_[0], 0, 0);
	}
}

}
