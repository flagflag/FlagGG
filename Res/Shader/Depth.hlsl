#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 position : POSITION;
    #ifdef SKINNED
        float4 blendWeights : BLEND_WEIGHTS;
        int4 blendIndices : BLEND_INDICES;
    #endif
    };
#endif

struct PixelInput
{
	float4 position : SV_POSITION;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
    #ifdef STATIC
        float4x3 iWorldMatrix = worldMatrix;
    #else
        float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
    #endif
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, iWorldMatrix);
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPosition;

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        return float4(1.0, 1.0, 1.0, 1.0);
    }
#endif

