using System;
using UnityEngine.Bindings;
using UnityEngine.Scripting;
using UnityEngine.Scripting.APIUpdating;

namespace UnityEngine
{
    public enum FilterMode
    {
        Point = 0,
        Bilinear = 1,
        Trilinear = 2,
    }

    public enum TextureWrapMode
    {
        Repeat = 0,
        Clamp = 1,
        Mirror = 2,
        MirrorOnce = 3,
    }

    public enum TextureFormat
    {
        Alpha8 = 1,
        ARGB4444 = 2,
        RGB24 = 3,
        RGBA32 = 4,
        ARGB32 = 5,
        RGB565 = 7,
        R16 = 9,
        DXT1 = 10,
        DXT5 = 12,
        RGBA4444 = 13,
        BGRA32 = 14,

        RHalf = 15,
        RGHalf = 16,
        RGBAHalf = 17,
        RFloat = 18,
        RGFloat = 19,
        RGBAFloat = 20,

        YUY2 = 21,
        RGB9e5Float = 22,

        BC4 = 26,
        BC5 = 27,
        BC6H = 24,
        BC7 = 25,

#if ENABLE_CRUNCH_TEXTURE_COMPRESSION
        DXT1Crunched = 28,
        DXT5Crunched = 29,
#endif

        PVRTC_RGB2 = 30,
        PVRTC_RGBA2 = 31,
        PVRTC_RGB4 = 32,
        PVRTC_RGBA4 = 33,
        ETC_RGB4 = 34,

#if UNITY_EDITOR
        [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Obsolete("Enum member TextureFormat.ATC_RGB4 has been deprecated. Use ETC_RGB4 instead (UnityUpgradable) -> ETC_RGB4", true)]
        ATC_RGB4 = -127,

        [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Obsolete("Enum member TextureFormat.ATC_RGBA8 has been deprecated. Use ETC2_RGBA8 instead (UnityUpgradable) -> ETC2_RGBA8", true)]
        ATC_RGBA8 = -127,
#endif

        EAC_R = 41,
        EAC_R_SIGNED = 42,
        EAC_RG = 43,
        EAC_RG_SIGNED = 44,
        ETC2_RGB = 45,
        ETC2_RGBA1 = 46,
        ETC2_RGBA8 = 47,

        ASTC_4x4 = 48,
        ASTC_5x5 = 49,
        ASTC_6x6 = 50,
        ASTC_8x8 = 51,
        ASTC_10x10 = 52,
        ASTC_12x12 = 53,

        // Nintendo 3DS
        [System.Obsolete("Nintendo 3DS is no longer supported.")]
        ETC_RGB4_3DS = 60,
        [System.Obsolete("Nintendo 3DS is no longer supported.")]
        ETC_RGBA8_3DS = 61,

        RG16 = 62,
        R8 = 63,

#if ENABLE_CRUNCH_TEXTURE_COMPRESSION
        ETC_RGB4Crunched = 64,
        ETC2_RGBA8Crunched = 65,
#endif

        ASTC_HDR_4x4 = 66,
        ASTC_HDR_5x5 = 67,
        ASTC_HDR_6x6 = 68,
        ASTC_HDR_8x8 = 69,
        ASTC_HDR_10x10 = 70,
        ASTC_HDR_12x12 = 71,

        // please note that obsolete attrs are currently disabled because we have tests that checks for "no warnings"
        // yet at the same time there are packages that reference old ASTC enums.
        // hence the only way is to go to trunk -> fix packages -> obsolete

        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGB_4x4 has been deprecated. Use ASTC_4x4 instead (UnityUpgradable) -> ASTC_4x4")]
        ASTC_RGB_4x4 = 48,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGB_5x5 has been deprecated. Use ASTC_5x5 instead (UnityUpgradable) -> ASTC_5x5")]
        ASTC_RGB_5x5 = 49,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGB_6x6 has been deprecated. Use ASTC_6x6 instead (UnityUpgradable) -> ASTC_6x6")]
        ASTC_RGB_6x6 = 50,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGB_8x8 has been deprecated. Use ASTC_8x8 instead (UnityUpgradable) -> ASTC_8x8")]
        ASTC_RGB_8x8 = 51,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGB_10x10 has been deprecated. Use ASTC_10x10 instead (UnityUpgradable) -> ASTC_10x10")]
        ASTC_RGB_10x10 = 52,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGB_12x12 has been deprecated. Use ASTC_12x12 instead (UnityUpgradable) -> ASTC_12x12")]
        ASTC_RGB_12x12 = 53,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGBA_4x4 has been deprecated. Use ASTC_4x4 instead (UnityUpgradable) -> ASTC_4x4")]
        ASTC_RGBA_4x4 = 54,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGBA_5x5 has been deprecated. Use ASTC_5x5 instead (UnityUpgradable) -> ASTC_5x5")]
        ASTC_RGBA_5x5 = 55,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGBA_6x6 has been deprecated. Use ASTC_6x6 instead (UnityUpgradable) -> ASTC_6x6")]
        ASTC_RGBA_6x6 = 56,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGBA_8x8 has been deprecated. Use ASTC_8x8 instead (UnityUpgradable) -> ASTC_8x8")]
        ASTC_RGBA_8x8 = 57,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGBA_10x10 has been deprecated. Use ASTC_10x10 instead (UnityUpgradable) -> ASTC_10x10")]
        ASTC_RGBA_10x10 = 58,
        // [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        // [System.Obsolete("Enum member TextureFormat.ASTC_RGBA_12x12 has been deprecated. Use ASTC_12x12 instead (UnityUpgradable) -> ASTC_12x12")]
        ASTC_RGBA_12x12 = 59,

#if UNITY_EDITOR
        [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Obsolete("Enum member TextureFormat.PVRTC_2BPP_RGB has been deprecated. Use PVRTC_RGB2 instead (UnityUpgradable) -> PVRTC_RGB2", true)]
        PVRTC_2BPP_RGB = -127,

        [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Obsolete("Enum member TextureFormat.PVRTC_2BPP_RGBA has been deprecated. Use PVRTC_RGBA2 instead (UnityUpgradable) -> PVRTC_RGBA2", true)]
        PVRTC_2BPP_RGBA = -127,

        [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Obsolete("Enum member TextureFormat.PVRTC_4BPP_RGB has been deprecated. Use PVRTC_RGB4 instead (UnityUpgradable) -> PVRTC_RGB4", true)]
        PVRTC_4BPP_RGB = -127,

        [System.ComponentModel.EditorBrowsable(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Obsolete("Enum member TextureFormat.PVRTC_4BPP_RGBA has been deprecated. Use PVRTC_RGBA4 instead (UnityUpgradable) -> PVRTC_RGBA4", true)]
        PVRTC_4BPP_RGBA = -127,
#endif
    }

    public enum ColorSpace
    {
        Uninitialized = -1,
        Gamma = 0,
        Linear = 1
    }

    [UsedByNativeCode]
    public enum GraphicsDeviceType
    {
        [System.Obsolete("OpenGL2 is no longer supported in Unity 5.5+")]
        OpenGL2 = 0,
        [System.Obsolete("Direct3D 9 is no longer supported in Unity 2017.2+")]
        Direct3D9 = 1,
        Direct3D11 = 2,
        [System.Obsolete("PS3 is no longer supported in Unity 5.5+")]
        PlayStation3 = 3,
        Null = 4,
        [System.Obsolete("Xbox360 is no longer supported in Unity 5.5+")]
        Xbox360 = 6,
        OpenGLES2 = 8,
        OpenGLES3 = 11,
        [System.Obsolete("PVita is no longer supported as of Unity 2018")]
        PlayStationVita = 12,
        PlayStation4 = 13,
        XboxOne = 14,
        [System.Obsolete("PlayStationMobile is no longer supported in Unity 5.3+")]
        PlayStationMobile = 15,
        Metal = 16,
        OpenGLCore = 17,
        Direct3D12 = 18,
        [System.Obsolete("Nintendo 3DS support is unavailable since 2018.1")]
        N3DS = 19,
        Vulkan = 21,
        Switch = 22,
        XboxOneD3D12 = 23
    }

    public enum RenderTextureFormat
    {
        ARGB32 = 0,
        Depth = 1,
        ARGBHalf = 2,
        Shadowmap = 3,
        RGB565 = 4,
        ARGB4444 = 5,
        ARGB1555 = 6,
        Default = 7,
        ARGB2101010 = 8,
        DefaultHDR = 9,
        ARGB64 = 10,
        ARGBFloat = 11,
        RGFloat = 12,
        RGHalf = 13,
        RFloat = 14,
        RHalf = 15,
        R8 = 16,
        ARGBInt = 17,
        RGInt = 18,
        RInt = 19,
        BGRA32 = 20,
        // kRTFormatVideo = 21,
        RGB111110Float = 22,
        RG32 = 23,
        RGBAUShort = 24,
        RG16 = 25,
        BGRA10101010_XR = 26,
        BGR101010_XR = 27,
        R16 = 28,
    }

    public enum CompareFunction
    {
        Disabled = 0,
        Never = 1,
        Less = 2,
        Equal = 3,
        LessEqual = 4,
        Greater = 5,
        NotEqual = 6,
        GreaterEqual = 7,
        Always = 8
    }

    public enum StencilOp
    {
        Keep = 0,
        Zero = 1,
        Replace = 2,
        IncrementSaturate = 3,
        DecrementSaturate = 4,
        Invert = 5,
        IncrementWrap = 6,
        DecrementWrap = 7
    }
}
