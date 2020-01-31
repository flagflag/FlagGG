#ifdef VERTEX
    struct VertexInput
    {
        float3 pos : POSITION;
        float2 tex : TEXCOORD;
        int4 color : COLOR;
    };
#else
    Texture2D colorMap : register(t0);
    SamplerState colorSampler : register(s0);
#endif

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        PixelInput output;
        output.pos = float4(input.pos, 1.0);
        output.tex = input.tex;
        output.color = input.color;
        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float2 tex;
        tex.x = input.tex.x;
        tex.y = 1.0 - input.tex.y;
        float4 color = float4(colorMap.Sample(colorSampler, tex).rgb, 1.0);
        return color;
    }
#endif
