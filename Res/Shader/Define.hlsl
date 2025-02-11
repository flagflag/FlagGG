#ifdef VERTEX
    cbuffer EngineParam : register(b0)
    {
        float4x3 worldMatrix;
        float4x3 viewMatrix;
        float4x4 projMatrix;
        float4x4 projviewMatrix;
        float4x3 invViewMatrix;
        float3 frustumSize;
        float nearClip;
        float farClip;
        float deltaTime;
        float elapsedTime;
        float3 cameraPos;
        float3 lightPos;
        float3 lightDir;
        float4 lightColor;
        float4x3 lightViewMatrix;
        float4x4 lightProjviewMatrix;
    }
    #ifdef SKINNED
        cbuffer SkinMatrixBuffer : register(b1)
        {
            uniform float4x3 skinMatrices[150];
        }
    #endif
#elif defined(PIXEL)
    cbuffer EngineParam : register(b0)
    {
        float4x3 viewMatrix;
        float4x4 projMatrix;
        float4x4 projviewMatrix;
        float nearClip;
        float farClip;
        float4 depthReconstruct;
        float4 screenPositionScaleBias;
        float deltaTime;
        float elapsedTime;
        uint frameNumber;
        uint frameNumberMod8;
        float3 cameraPos;
        float3 lightPos;
        float3 lightDir;
        float4 lightColor;
        float4x3 lightViewMatrix;
        float4x4 lightProjviewMatrix;
        float2 shadowMapPixelTexels;
        float4 envCubeAngle;
        float shIntensity;
        float iblIntensity;
        float ambientOcclusionIntensity;
        // SH
        float4 SHAr;
        float4 SHAg;
        float4 SHAb;
        float4 SHBr;
        float4 SHBg;
        float4 SHBb;
        float4 SHC;
    }
    #define Sample2D(name, tex) name##Map.Sample(name##Sampler, tex)
#endif

#define M_PI 3.14159265358979323846
#define M_INV_PI 0.31830988618
#define M_EPSILON 0.0001
