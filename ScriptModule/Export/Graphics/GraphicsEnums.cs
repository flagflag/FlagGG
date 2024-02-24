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
}

namespace UnityEngine.Rendering
{
    // Match IndexFormat on C++ side
    public enum IndexFormat
    {
        UInt16 = 0,
        UInt32 = 1,
    }

    [Flags]
    public enum MeshUpdateFlags
    {
        Default = 0,
        DontValidateIndices = 1 << 0,
        DontResetBoneBounds = 1 << 1,
        DontNotifyMeshUsers = 1 << 2,
        DontRecalculateBounds = 1 << 3,
    }

    // Match VertexFormat on C++ side
    public enum VertexAttributeFormat
    {
        Float32 = 0,
        Float16 = 1,
        UNorm8 = 2,
        SNorm8 = 3,
        UNorm16 = 4,
        SNorm16 = 5,
        UInt8 = 6,
        SInt8 = 7,
        UInt16 = 8,
        SInt16 = 9,
        UInt32 = 10,
        SInt32 = 11,
    }

    //Keep in sync with ShaderChannel in GfxDeviceTypes.h
    [MovedFrom("UnityEngine.Experimental.Rendering")]
    public enum VertexAttribute
    {
        Position = 0, // Vertex (vector3)
        Normal,       // Normal (vector3)
        Tangent,      // Tangent (vector4)
        Color,        // Vertex color
        TexCoord0,    // Texcoord 0
        TexCoord1,    // Texcoord 1
        TexCoord2,    // Texcoord 2
        TexCoord3,    // Texcoord 3
        TexCoord4,    // Texcoord 4
        TexCoord5,    // Texcoord 5
        TexCoord6,    // Texcoord 6
        TexCoord7,    // Texcoord 7
        BlendWeight,
        BlendIndices,
    }

    // Match Camera::OpaqueSortMode on C++ side
    public enum OpaqueSortMode
    {
        Default = 0,
        FrontToBack = 1,
        NoDistanceSort = 2
    }

    // Match RenderLoopEnums.h on C++ side
    public enum RenderQueue
    {
        Background = 1000,
        Geometry = 2000,
        AlphaTest = 2450, // we want it to be in the end of geometry queue
        GeometryLast = 2500, // last queue that is considered "opaque" by Unity
        Transparent = 3000,
        Overlay = 4000,
    }

    // Make sure the values are in sync with the native side!
    public enum RenderBufferLoadAction
    {
        Load = 0,
        Clear = 1,
        DontCare = 2,
    }

    // Make sure the values are in sync with the native side!
    public enum RenderBufferStoreAction
    {
        Store = 0,
        Resolve = 1, // Resolve the MSAA surface (currently only works with RenderPassSetup)
        StoreAndResolve = 2, // Resolve the MSAA surface into the resolve target, but also store the MSAA version
        DontCare = 3,
    }

    public enum BlendMode
    {
        Zero = 0,
        One = 1,
        DstColor = 2,
        SrcColor = 3,
        OneMinusDstColor = 4,
        SrcAlpha = 5,
        OneMinusSrcColor = 6,
        DstAlpha = 7,
        OneMinusDstAlpha = 8,
        SrcAlphaSaturate = 9,
        OneMinusSrcAlpha = 10
    }

    public enum BlendOp
    {
        Add = 0,
        Subtract = 1,
        ReverseSubtract = 2,
        Min = 3,
        Max = 4,
        LogicalClear = 5,
        LogicalSet = 6,
        LogicalCopy = 7,
        LogicalCopyInverted = 8,
        LogicalNoop = 9,
        LogicalInvert = 10,
        LogicalAnd = 11,
        LogicalNand = 12,
        LogicalOr = 13,
        LogicalNor = 14,
        LogicalXor = 15,
        LogicalEquivalence = 16,
        LogicalAndReverse = 17,
        LogicalAndInverted = 18,
        LogicalOrReverse = 19,
        LogicalOrInverted = 20,
        Multiply = 21,
        Screen = 22,
        Overlay = 23,
        Darken = 24,
        Lighten = 25,
        ColorDodge = 26,
        ColorBurn = 27,
        HardLight = 28,
        SoftLight = 29,
        Difference = 30,
        Exclusion = 31,
        HSLHue = 32,
        HSLSaturation = 33,
        HSLColor = 34,
        HSLLuminosity = 35,
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

    public enum CullMode
    {
        Off = 0,
        Front = 1,
        Back = 2
    }

    [Flags]
    public enum ColorWriteMask
    {
        Alpha = 1,
        Blue = 2,
        Green = 4,
        Red = 8,
        All = 15
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
