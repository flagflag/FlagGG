#include "Shader/Define.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float3 position : POSITION;
        float2 texcoord : TEXCOORD;
        float4 color    : COLOR;
    };
#else
    Texture2D colorMap : register(t0);
    SamplerState colorSampler : register(s0);
#endif

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        float3 worldPosition = mul(float4(input.position, 1.0), worldMatrix);
        PixelInput output;
        output.position = float4(worldPosition, 1.0);
        output.texcoord = input.texcoord;
        output.color    = input.color;
        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float2 texcoord;
        texcoord.x = input.texcoord.x;
        texcoord.y = 1.0 - input.texcoord.y;
        float4 color = float4(colorMap.Sample(colorSampler, texcoord).rgb, 1.0) * input.color;
        return color;
    }
#endif
