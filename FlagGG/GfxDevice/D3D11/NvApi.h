#pragma once

#include "Container/Str.h"
#include <stdint.h>
#include <Windows.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D12Device;
struct ID3D12CommandList;

namespace FlagGG
{
struct NvPhysicalGpuHandle;
struct NvAftermathContextHandle;

struct NvAftermathDeviceStatus
{
    enum Enum
    {
        Active,
        Timeout,
        OutOfMemory,
        PageFault,
        Unknown,
        NotInitialized
    };
};

typedef void (*PFN_NVAPI_MULTIDRAWINDIRECT)(ID3D11DeviceContext* deviceCtx, uint32_t numDrawIndirect, ID3D11Buffer* ptr, uint32_t offset, uint32_t stride);

struct NvApi
{
    NvApi();

    void Init();

    void Shutdown();

    bool IsInitialized() const { return NULL != nvGpu_; }

    void GetMemoryInfo(int64_t& gpuMemoryUsed, int64_t& gpuMemoryMax);

    bool LoadAftermath();

    bool InitAftermath(const ID3D11Device* device, const ID3D11DeviceContext* deviceCtx);

    bool InitAftermath(const ID3D12Device* device, const ID3D12CommandList* commandList);

    NvAftermathDeviceStatus::Enum GetDeviceStatus() const;

    void ShutdownAftermath();

    void SetMarker(const String& marker);

    void* nvApiDll_;
    NvPhysicalGpuHandle* nvGpu_;

    void* nvAftermathDll_;
    NvAftermathContextHandle* aftermathHandle_;

    PFN_NVAPI_MULTIDRAWINDIRECT nvApiD3D11MultiDrawInstancedIndirect;
    PFN_NVAPI_MULTIDRAWINDIRECT nvApiD3D11MultiDrawIndexedInstancedIndirect;
};

}
