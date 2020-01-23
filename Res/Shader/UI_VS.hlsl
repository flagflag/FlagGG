
struct VertexInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.pos = float4(input.pos, 1.0);
    output.tex = input.tex;
    output.color = input.color;
    return output;
}

