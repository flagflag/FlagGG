
struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    int4 color : COLOR;
};

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = float4(1, 0, 0, 1);
    return color;
}
