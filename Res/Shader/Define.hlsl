#ifdef VERTEX
    cbuffer MatrixBuffer : register(b0)
    {
        float4x3 worldMatrix;
        float4x3 viewMatrix;
        float4x4 projviewMatrix;
    }
    #ifdef SKINNED
        cbuffer SkinMatrixBuffer : register(b1)
        {
            uniform float4x3 skinMatrices[64];
        }
    #endif
    cbuffer ParamBuffer : register(b2)
    {
        float deltaTime;
        float elapsedTime;
        float3 cameraPos;
        float3 lightPos;
        float3 lightDir;
        float4x3 lightViewMatrix;
        float4x4 lightProjviewMatrix;
    }
#else
    cbuffer ParamBuffer : register(b0)
    {
        float deltaTime;
        float elapsedTime;
        float3 cameraPos;
        float3 lightPos;
        float3 lightDir;
        float4x3 lightViewMatrix;
        float4x4 lightProjviewMatrix;
        float2 shadowMapPixelTexels;
    }
    #define Sample2D(name, tex) name##Map.Sample(name##Sampler, tex)
#endif
