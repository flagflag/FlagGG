#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 position : POSITION;
        float2 texcoord : TEXCOORD;
    #ifdef SKINNED
        float4 blendWeights : BLEND_WEIGHTS;
        int4 blendIndices : BLEND_INDICES;
    #endif
    #ifdef INSTANCE
        float4 instanceData0 : INSTANCE0;
        float4 instanceData1 : INSTANCE1;
        float4 instanceData2 : INSTANCE2;
    #endif
    };
#endif

struct PixelInput
{
	float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
    #ifdef STATIC
        #ifdef INSTANCE
            float4x3 iWorldMatrix = float4x3(
                float3(input.instanceData0.x, input.instanceData1.x, input.instanceData2.x),
                float3(input.instanceData0.y, input.instanceData1.y, input.instanceData2.y),
                float3(input.instanceData0.z, input.instanceData1.z, input.instanceData2.z),
                float3(input.instanceData0.w, input.instanceData1.w, input.instanceData2.w)
                );
        #else
            float4x3 iWorldMatrix = worldMatrix;
        #endif
    #else
        float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
    #endif
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, iWorldMatrix);
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPosition;
        output.texcoord = input.texcoord;

        return output;
    }
#else
    #ifdef ALPHAMASK
        Texture2D colorMap : register(t0);
        SamplerState colorSampler : register(s0);
    #endif

    float4 PS(PixelInput input) : SV_TARGET
    {
    #ifdef ALPHAMASK
        float4 texDiff = colorMap.Sample(colorSampler, input.texcoord);
        if (texDiff.a < 0.5)
            discard;
    #endif
        return float4(1.0, 1.0, 1.0, 1.0);
    }
#endif

