#include "GfxBufferD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxD3D11Defines.h"
#include "Log.h"
#include "Core/CryAssert.h"

namespace FlagGG
{

static D3D11_USAGE d3d11Usage[]=
{
	D3D11_USAGE_IMMUTABLE,
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

}

void GfxBufferD3D11::Apply(const void* initialDataPtr)
{
	if (d3d11Buffer_)
	{
		D3D11_SAFE_RELEASE(d3d11Buffer_);
	}

	UINT bindFlags = 0;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_VERTEX)
		bindFlags |= D3D11_BIND_VERTEX_BUFFER;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_INDEX)
		bindFlags |= D3D11_BIND_INDEX_BUFFER;
	if (gfxBufferDesc_.bindFlags_ & BUFFER_UNIFORM)
		bindFlags |= D3D11_BIND_CONSTANT_BUFFER;

	UINT accessFlags = 0;
	if (gfxBufferDesc_.accessFlags_ & BUFFER_WRITE)
		accessFlags |= D3D11_CPU_ACCESS_WRITE;
	if (gfxBufferDesc_.accessFlags_ & BUFFER_READ)
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

	D3D11_SUBRESOURCE_DATA d3d11SubData;
	d3d11SubData.pSysMem = initialDataPtr;
	d3d11SubData.SysMemPitch = 0;
	d3d11SubData.SysMemSlicePitch = 0;

	HRESULT hr = GfxDeviceD3D11::Instance()->GetD3D11Device()->CreateBuffer(&desc, &d3d11SubData, &d3d11Buffer_);
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
	if (gfxBufferDesc_.usage_ == BUFFER_STATIC)
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
	if (gfxBufferDesc_.usage_ == BUFFER_STATIC)
	{
		CRY_ASSERT(false);
		return;
	}

	if (offset + size > gfxBufferDesc_.size_)
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

	D3D11_MAPPED_SUBRESOURCE mappedData;
	memset(&mappedData, 0, sizeof mappedData);
	HRESULT hr = GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->Map(d3d11Buffer_, 0, d3d11Map[gfxBufferDesc_.usage_], 0, &mappedData);
	if (FAILED(hr) || !mappedData.pData)
	{
		FLAGGG_LOG_ERROR("Failed to Map buffer data.");
		return nullptr;
	}
	return static_cast<char*>(mappedData.pData) + offset;
}

void GfxBufferD3D11::EndWrite(UInt32 bytesWritten)
{
	if (!d3d11Buffer_)
	{
		CRY_ASSERT(false);
		return;
	}

	GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->Unmap(d3d11Buffer_, 0);
}

}
