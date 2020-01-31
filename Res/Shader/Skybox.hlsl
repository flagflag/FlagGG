#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 pos : POSITION;
    };
#else
    TextureCube colorMap : register(t0);
    SamplerState colorSampler : register(s0);
#endif

struct PixelInput
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.pos.w = 1.0;
        float3 worldPos = mul(input.pos, worldMatrix);
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.pos = clipPos;
        output.tex = input.pos.xyz;

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float4 color = colorMap.Sample(colorSampler, input.tex);

        return color;
    }
#endif
