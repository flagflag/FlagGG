#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 position : POSITION;
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
        float4x3 iWorldMatrix = worldMatrix;
        input.position.w = 1.0;
        float3 worldPos = mul(input.position, iWorldMatrix);
        float4 clipPos  = mul(float4(worldPos, 1.0), projviewMatrix);

        PixelInput output;
        output.position = clipPos;
        output.texcoord = GetQuadTexCoord(clipPos);
        return output;
    }
#endif

#ifdef PIXEL
    Texture2D GBuffer0Map : register(t0);
    Texture2D GBuffer1Map : register(t1);
    Texture2D GBuffer2Map : register(t2);

    SamplerState GBuffer0Sampler : register(s0);
    SamplerState GBuffer1Sampler : register(s1);
    SamplerState GBuffer2Sampler : register(s2);

    float4 PS(PixelInput input) : SV_TARGET
    {
        float4 color = Sample2D(GBuffer2, input.texcoord);
        return float4(color.rgb, 1.0);
    }
#endif
