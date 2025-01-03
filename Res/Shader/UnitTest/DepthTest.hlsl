#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 position  : POSITION;
    };
#endif

struct PixelInput
{
	float4 position  : SV_POSITION;
    float4 screenPos : SCREENPOS;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, worldMatrix);
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);

        PixelInput output;
        output.position = clipPosition;
        output.screenPos = GetScreenPos(clipPosition);
        return output;
    }
#else
    Texture2D depthBuffer : register(t0);
    SamplerState depthBufferSampler : register(s0);

    float4 PS(PixelInput input) : SV_TARGET
    {
        // float depth = ReconstructDepth(depthBuffer.Sample(depthBufferSampler, input.screenPos.xy / input.screenPos.w).r);
        // float depth = depthBuffer.Sample(depthBufferSampler, input.screenPos.xy / input.screenPos.w).r;
        float depth = LinearizeDepth(depthBuffer.Sample(depthBufferSampler, input.screenPos.xy / input.screenPos.w).r, nearClip, farClip);
	    return float4(depth, 0.0, 0.0, 1.0);
    }
#endif
