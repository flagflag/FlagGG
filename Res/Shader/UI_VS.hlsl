
struct VertexInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

struct PixelInput
{
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.tex = input.tex;
    output.color = input.color;
    return output;
}

