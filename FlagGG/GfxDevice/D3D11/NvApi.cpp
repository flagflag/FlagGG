#include "NvApi.h"
#include "Core/BaseTypes.h"
#include "Log.h"

namespace FlagGG
{
/*
 * NVAPI
 *
 * Reference(s):
 * - https://web.archive.org/web/20181126035649/https://docs.nvidia.com/gameworks/content/gameworkslibrary/coresdk/nvapi/index.html
 * - https://web.archive.org/web/20181126035710/https://github.com/jNizM/AHK_NVIDIA_NvAPI/blob/master/info/NvAPI_IDs.txt
 */

struct NvPhysicalGpuHandle;

#define NVAPI_MAX_PHYSICAL_GPUS 64

#if PLATFORM_WINDOWS
#	define NVAPICALL __cdecl
#else
#	define NVAPICALL
#endif

enum NvApiStatus
{
    NVAPI_OK    =  0,
    NVAPI_ERROR = -1,
};

struct NvMemoryInfoV2
{
    NvMemoryInfoV2()
        : version(sizeof(NvMemoryInfoV2) | (2 << 16) )
    {
    }

    uint32_t version;
    uint32_t dedicatedVideoMemory;
    uint32_t availableDedicatedVideoMemory;
    uint32_t systemVideoMemory;
    uint32_t sharedSystemMemory;
    uint32_t curAvailableDedicatedVideoMemory;
};

typedef void*       (NVAPICALL* PFN_NVAPI_QUERYINTERFACE)(uint32_t _functionOffset);
typedef NvApiStatus (NVAPICALL* PFN_NVAPI_INITIALIZE)();
typedef NvApiStatus (NVAPICALL* PFN_NVAPI_UNLOAD)();
typedef NvApiStatus (NVAPICALL* PFN_NVAPI_ENUMPHYSICALGPUS)(NvPhysicalGpuHandle* _handle[NVAPI_MAX_PHYSICAL_GPUS], uint32_t* _gpuCount);
typedef NvApiStatus (NVAPICALL* PFN_NVAPI_GPUGETMEMORYINFO)(NvPhysicalGpuHandle* _handle, NvMemoryInfoV2* _memoryInfo);
typedef NvApiStatus (NVAPICALL* PFN_NVAPI_GPUGETFULLNAME)(NvPhysicalGpuHandle* _physicalGpu, char _name[64]);

#define NVAPI_INITIALIZE                        UINT32_C(0x0150e828)
#define NVAPI_UNLOAD                            UINT32_C(0xd22bdd7e)
#define NVAPI_ENUMPHYSICALGPUS                  UINT32_C(0xe5ac921f)
#define NVAPI_GPUGETMEMORYINFO                  UINT32_C(0x07f9b368)
#define NVAPI_GPUGETFULLNAME                    UINT32_C(0xceee8e9f)
#define NVAPI_MULTIDRAWINSTANCEDINDIRECT        UINT32_C(0xd4e26bbf)
#define NVAPI_MULTIDRAWINDEXEDINSTANCEDINDIRECT UINT32_C(0x59e890f9)

static PFN_NVAPI_QUERYINTERFACE   nvApiQueryInterface;
static PFN_NVAPI_INITIALIZE       nvApiInitialize;
static PFN_NVAPI_UNLOAD           nvApiUnload;
static PFN_NVAPI_ENUMPHYSICALGPUS nvApiEnumPhysicalGPUs;
static PFN_NVAPI_GPUGETMEMORYINFO nvApiGpuGetMemoryInfo;
static PFN_NVAPI_GPUGETFULLNAME   nvApiGpuGetFullName;

/*
 * NVIDIA Aftermath
 *
 * Reference(s):
 * - https://web.archive.org/web/20181126035743/https://developer.nvidia.com/nvidia-aftermath
 */

typedef int32_t (*PFN_NVAFTERMATH_DX11_INITIALIZE)(int32_t _version, int32_t _flags, const ID3D11Device* _device);
typedef int32_t (*PFN_NVAFTERMATH_DX11_CREATECONTEXTHANDLE)(const ID3D11DeviceContext* _deviceCtx, NvAftermathContextHandle** _outContextHandle);
typedef int32_t (*PFN_NVAFTERMATH_DX12_INITIALIZE)(int32_t _version, int32_t _flags, const ID3D12Device* _device);
typedef int32_t (*PFN_NVAFTERMATH_DX12_CREATECONTEXTHANDLE)(const ID3D12CommandList* _commandList, NvAftermathContextHandle** _outContextHandle);
typedef int32_t (*PFN_NVAFTERMATH_RELEASECONTEXTHANDLE)(const NvAftermathContextHandle* _contextHandle);
typedef int32_t (*PFN_NVAFTERMATH_SETEVENTMARKER)(const NvAftermathContextHandle* _contextHandle, const void* _markerData, uint32_t _markerSize);
typedef int32_t (*PFN_NVAFTERMATH_GETDATA)(uint32_t _numContexts, const NvAftermathContextHandle** _contextHandles, void* _outContextData);
typedef int32_t (*PFN_NVAFTERMATH_GETDEVICESTATUS)(void* _outStatus);
typedef int32_t (*PFN_NVAFTERMATH_GETPAGEFAULTINFORMATION)(void* _outPageFaultInformation);

static PFN_NVAFTERMATH_DX11_INITIALIZE          nvAftermathDx11Initialize;
static PFN_NVAFTERMATH_DX11_CREATECONTEXTHANDLE nvAftermathDx11CreateContextHandle;
static PFN_NVAFTERMATH_DX12_INITIALIZE          nvAftermathDx12Initialize;
static PFN_NVAFTERMATH_DX12_CREATECONTEXTHANDLE nvAftermathDx12CreateContextHandle;
static PFN_NVAFTERMATH_RELEASECONTEXTHANDLE     nvAftermathReleaseContextHandle;
static PFN_NVAFTERMATH_SETEVENTMARKER           nvAftermathSetEventMarker;
static PFN_NVAFTERMATH_GETDATA                  nvAftermathGetData;
static PFN_NVAFTERMATH_GETDEVICESTATUS          nvAftermathGetDeviceStatus;
static PFN_NVAFTERMATH_GETPAGEFAULTINFORMATION  nvAftermathGetPageFaultInformation;

NvApi::NvApi()
    : nvApiDll_(NULL)
    , nvGpu_(NULL)
    , nvAftermathDll_(NULL)
    , aftermathHandle_(NULL)
{
}

void NvApi::Init()
{
    nvGpu_ = NULL;
    nvApiDll_ = ::LoadLibraryA(
        "nvapi"
#if _WIN64
        "64"
#endif
        ".dll"
        );

    if (NULL != nvApiDll_)
    {
        nvApiQueryInterface = (PFN_NVAPI_QUERYINTERFACE)::GetProcAddress(nvApiDll_, "nvapi_QueryInterface");

        bool initialized = NULL != nvApiQueryInterface;

        if (initialized)
        {
            nvApiInitialize       = (PFN_NVAPI_INITIALIZE      )nvApiQueryInterface(NVAPI_INITIALIZE);
            nvApiUnload           = (PFN_NVAPI_UNLOAD          )nvApiQueryInterface(NVAPI_UNLOAD);
            nvApiEnumPhysicalGPUs = (PFN_NVAPI_ENUMPHYSICALGPUS)nvApiQueryInterface(NVAPI_ENUMPHYSICALGPUS);
            nvApiGpuGetMemoryInfo = (PFN_NVAPI_GPUGETMEMORYINFO)nvApiQueryInterface(NVAPI_GPUGETMEMORYINFO);
            nvApiGpuGetFullName   = (PFN_NVAPI_GPUGETFULLNAME  )nvApiQueryInterface(NVAPI_GPUGETFULLNAME);

            nvApiD3D11MultiDrawInstancedIndirect        = (PFN_NVAPI_MULTIDRAWINDIRECT)nvApiQueryInterface(NVAPI_MULTIDRAWINSTANCEDINDIRECT);
            nvApiD3D11MultiDrawIndexedInstancedIndirect = (PFN_NVAPI_MULTIDRAWINDIRECT)nvApiQueryInterface(NVAPI_MULTIDRAWINDEXEDINSTANCEDINDIRECT);

            initialized = true
                && NULL != nvApiInitialize
                && NULL != nvApiUnload
                && NULL != nvApiEnumPhysicalGPUs
                && NULL != nvApiGpuGetMemoryInfo
                && NULL != nvApiGpuGetFullName
                && NVAPI_OK == nvApiInitialize()
                ;

            if (initialized)
            {
                NvPhysicalGpuHandle* physicalGpus[NVAPI_MAX_PHYSICAL_GPUS];
                uint32_t numGpus = 0;
                nvApiEnumPhysicalGPUs(physicalGpus, &numGpus);

                initialized = 0 < numGpus;
                if (initialized)
                {
                    nvGpu_ = physicalGpus[0];

                    initialized = NULL != nvGpu_;
                    if (initialized)
                    {
                        char name[64];
                        nvApiGpuGetFullName(nvGpu_, name);
                        FLAGGG_LOG_STD_DEBUG("%s", name);
                    }
                    else
                    {
                        nvApiUnload();
                    }
                }
            }
        }

        if (!initialized)
        {
            ::FreeLibrary(nvApiDll_);
            nvApiDll_ = NULL;
        }

        if (!initialized)
            FLAGGG_LOG_INFO("NVAPI supported.");
    }
}

void NvApi::Shutdown()
{
    if (NULL != nvGpu_)
    {
        nvApiUnload();
        nvGpu_ = NULL;
    }

    if (NULL != nvApiDll_)
    {
        ::FreeLibrary(nvApiDll_);
        nvApiDll_ = NULL;
    }

    ShutdownAftermath();
}

void NvApi::GetMemoryInfo(int64_t& gpuMemoryUsed, int64_t& gpuMemoryMax)
{
    if (NULL != nvGpu_)
    {
        NvMemoryInfoV2 memInfo;
        NvApiStatus status = nvApiGpuGetMemoryInfo(nvGpu_, &memInfo);
        if (NVAPI_OK == status)
        {
            gpuMemoryMax  = 1024 * int64_t(memInfo.availableDedicatedVideoMemory);
            gpuMemoryUsed = 1024 * int64_t(memInfo.availableDedicatedVideoMemory - memInfo.curAvailableDedicatedVideoMemory);
        }
    }
    else
    {
        gpuMemoryMax  = -INT64_MAX;
        gpuMemoryUsed = -INT64_MAX;
    }
}

bool NvApi::LoadAftermath()
{
    nvAftermathDll_ = ::LoadLibrary(
        "GFSDK_Aftermath_Lib."
#if !_WIN64
        "x86"
#else
        "x64"
#endif
        ".dll"
        );

    if (NULL != nvAftermathDll_)
    {
        nvAftermathDx11Initialize          = (PFN_NVAFTERMATH_DX11_INITIALIZE         )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_DX11_Initialize");
        nvAftermathDx11CreateContextHandle = (PFN_NVAFTERMATH_DX11_CREATECONTEXTHANDLE)::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_DX11_CreateContextHandle");
        nvAftermathDx12Initialize          = (PFN_NVAFTERMATH_DX12_INITIALIZE         )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_DX12_Initialize");
        nvAftermathDx12CreateContextHandle = (PFN_NVAFTERMATH_DX12_CREATECONTEXTHANDLE)::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_DX12_CreateContextHandle");
        nvAftermathReleaseContextHandle	   = (PFN_NVAFTERMATH_RELEASECONTEXTHANDLE    )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_ReleaseContextHandle");
        nvAftermathSetEventMarker          = (PFN_NVAFTERMATH_SETEVENTMARKER          )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_SetEventMarker");
        nvAftermathGetData                 = (PFN_NVAFTERMATH_GETDATA                 )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_GetData");
        nvAftermathGetDeviceStatus         = (PFN_NVAFTERMATH_GETDEVICESTATUS         )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_GetDeviceStatus");
        nvAftermathGetPageFaultInformation = (PFN_NVAFTERMATH_GETPAGEFAULTINFORMATION )::GetProcAddress(nvAftermathDll_, "GFSDK_Aftermath_GetPageFaultInformation");

        bool initialized = true
            && NULL != nvAftermathDx11Initialize
            && NULL != nvAftermathDx11CreateContextHandle
            && NULL != nvAftermathDx12Initialize
            && NULL != nvAftermathDx12CreateContextHandle
            && NULL != nvAftermathReleaseContextHandle
            && NULL != nvAftermathSetEventMarker
            && NULL != nvAftermathGetData
            && NULL != nvAftermathGetDeviceStatus
            && NULL != nvAftermathGetPageFaultInformation
            ;

        if (initialized)
        {
            return true;
        }

        ShutdownAftermath();
    }

    return false;
}

bool NvApi::InitAftermath(const ID3D11Device* device, const ID3D11DeviceContext* deviceCtx)
{
    if (LoadAftermath())
    {
        int32_t result;
        result = nvAftermathDx11Initialize(0x13, 1, device);
        if (1 == result)
        {
            result = nvAftermathDx11CreateContextHandle(deviceCtx, &aftermathHandle_);
            if (1 != result)
                FLAGGG_LOG_STD_WARN("NV Aftermath: nvAftermathDx12CreateContextHandle failed %x", result);

            if (1 == result)
            {
                return true;
            }
        }
        else
        {
            switch (result)
            {
            case int32_t(0xbad0000a): FLAGGG_LOG_STD_INFO("NV Aftermath: Debug layer not compatible with Aftermath."); break;
            default:                  FLAGGG_LOG_STD_INFO("NV Aftermath: Failed to initialize."); break;
            }
        }

        ShutdownAftermath();
    }

    return false;
}

bool NvApi::InitAftermath(const ID3D12Device* device, const ID3D12CommandList* commandList)
{
    if (LoadAftermath())
    {
        int32_t result;
        result = nvAftermathDx12Initialize(0x13, 1, device);
        if (1 == result)
        {
            result = nvAftermathDx12CreateContextHandle(commandList, &aftermathHandle_);
            if (1 != result)
                FLAGGG_LOG_STD_WARN("NV Aftermath: nvAftermathDx12CreateContextHandle failed %x", result);

            if (1 == result)
            {
                return true;
            }
        }
        else
        {
            switch (result)
            {
            case int32_t(0xbad0000a): FLAGGG_LOG_STD_INFO("NV Aftermath: Debug layer not compatible with Aftermath."); break;
            default:                  FLAGGG_LOG_STD_INFO("NV Aftermath: Failed to initialize."); break;
            }
        }

        ShutdownAftermath();
    }

    return false;
}

NvAftermathDeviceStatus::Enum NvApi::GetDeviceStatus() const
{
    if (NULL != aftermathHandle_)
    {
        int32_t status;
        nvAftermathGetDeviceStatus(&status);

        return NvAftermathDeviceStatus::Enum(status);
    }

    return NvAftermathDeviceStatus::NotInitialized;
}

void NvApi::ShutdownAftermath()
{
    if (NULL != nvAftermathDll_)
    {
        if (NULL != aftermathHandle_)
        {
            nvAftermathReleaseContextHandle(aftermathHandle_);
            aftermathHandle_ = NULL;
        }

        ::FreeLibrary(nvAftermathDll_);
        nvAftermathDll_ = NULL;
    }
}

void NvApi::SetMarker(const String& marker)
{
    if (NULL != aftermathHandle_)
    {
        ASSERT(nvAftermathSetEventMarker(aftermathHandle_, marker.CString(), marker.Length()));
    }
}

}
