Texture2D normalMap : register(t0);
SamplerState normalSampler : register(s0);

Texture2D envMap : register(t5);
SamplerState envSampler : register(s5);

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};

float4 PS(PixelInput input) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0);
}
