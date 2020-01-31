Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

float4 PS(PixelInput input) : SV_TARGET
{
    float2 tex;
    tex.x = input.tex.x;
    tex.y = 1.0 - input.tex.y;
    float4 color = float4(colorMap.Sample(colorSampler, tex).rgb, 1.0);
    return color;
}
