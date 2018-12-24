//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.h
//
// Functions for loading a DDS texture and creating a Direct3D 11 runtime resource for it
//
// Note these functions are useful as a light-weight runtime loader for DDS files. For
// a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma once
#endif

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#define DCOMMON_H_INCLUDED
#define NO_D3D11_DEBUG_NAME
#else
#include <d3d11.h>
#endif

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

namespace DirectX
{
    enum DDS_ALPHA_MODE
    {
        DDS_ALPHA_MODE_UNKNOWN       = 0,
        DDS_ALPHA_MODE_STRAIGHT      = 1,
        DDS_ALPHA_MODE_PREMULTIPLIED = 2,
        DDS_ALPHA_MODE_OPAQUE        = 3,
        DDS_ALPHA_MODE_CUSTOM        = 4,
    };

    // Standard version
    HRESULT CreateDDSTextureFromMemory( ID3D11Device* d3dDevice,
                                        const uint8_t* ddsData,
                                        size_t ddsDataSize,
                                        ID3D11Resource** texture,
                                        ID3D11ShaderResourceView** textureView,
                                        size_t maxsize = 0,
                                        DDS_ALPHA_MODE* alphaMode = nullptr
                                      );

    HRESULT CreateDDSTextureFromFile( ID3D11Device* d3dDevice,
                                      const wchar_t* szFileName,
                                      ID3D11Resource** texture,
                                      ID3D11ShaderResourceView** textureView,
                                      size_t maxsize = 0,
                                      DDS_ALPHA_MODE* alphaMode = nullptr
                                    );

    // Standard version with optional auto-gen mipmap support
    HRESULT CreateDDSTextureFromMemory( ID3D11Device* d3dDevice,
                                        ID3D11DeviceContext* d3dContext,
                                        const uint8_t* ddsData,
                                        size_t ddsDataSize,
                                        ID3D11Resource** texture,
                                        ID3D11ShaderResourceView** textureView,
                                        size_t maxsize = 0,
                                        DDS_ALPHA_MODE* alphaMode = nullptr
                                      );

    HRESULT CreateDDSTextureFromFile( ID3D11Device* d3dDevice,
                                      ID3D11DeviceContext* d3dContext,
                                      const wchar_t* szFileName,
                                      ID3D11Resource** texture,
                                      ID3D11ShaderResourceView** textureView,
                                      size_t maxsize = 0,
                                      DDS_ALPHA_MODE* alphaMode = nullptr
                                    );

    // Extended version
    HRESULT CreateDDSTextureFromMemoryEx( ID3D11Device* d3dDevice,
                                          const uint8_t* ddsData,
                                          size_t ddsDataSize,
                                          size_t maxsize,
                                          D3D11_USAGE usage,
                                          unsigned int bindFlags,
                                          unsigned int cpuAccessFlags,
                                          unsigned int miscFlags,
                                          bool forceSRGB,
                                          ID3D11Resource** texture,
                                          ID3D11ShaderResourceView** textureView,
                                          DDS_ALPHA_MODE* alphaMode = nullptr
                                      );

    HRESULT CreateDDSTextureFromFileEx( ID3D11Device* d3dDevice,
                                        const wchar_t* szFileName,
                                        size_t maxsize,
                                        D3D11_USAGE usage,
                                        unsigned int bindFlags,
                                        unsigned int cpuAccessFlags,
                                        unsigned int miscFlags,
                                        bool forceSRGB,
                                        ID3D11Resource** texture,
                                        ID3D11ShaderResourceView** textureView,
                                        DDS_ALPHA_MODE* alphaMode = nullptr
                                    );

    // Extended version with optional auto-gen mipmap support
    HRESULT CreateDDSTextureFromMemoryEx( ID3D11Device* d3dDevice,
                                          ID3D11DeviceContext* d3dContext,
                                          const uint8_t* ddsData,
                                          size_t ddsDataSize,
                                          size_t maxsize,
                                          D3D11_USAGE usage,
                                          unsigned int bindFlags,
                                          unsigned int cpuAccessFlags,
                                          unsigned int miscFlags,
                                          bool forceSRGB,
                                          ID3D11Resource** texture,
                                          ID3D11ShaderResourceView** textureView,
                                          DDS_ALPHA_MODE* alphaMode = nullptr
                                      );

    HRESULT CreateDDSTextureFromFileEx( ID3D11Device* d3dDevice,
                                        ID3D11DeviceContext* d3dContext,
                                        const wchar_t* szFileName,
                                        size_t maxsize,
                                        D3D11_USAGE usage,
                                        unsigned int bindFlags,
                                        unsigned int cpuAccessFlags,
                                        unsigned int miscFlags,
                                        bool forceSRGB,
                                        ID3D11Resource** texture,
                                        ID3D11ShaderResourceView** textureView,
                                        DDS_ALPHA_MODE* alphaMode = nullptr
                                    );
}