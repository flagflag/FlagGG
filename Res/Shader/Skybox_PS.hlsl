TextureCube colorMap : register(t0);
SamplerState colorSampler : register(s0);

struct PixelInput
{
	float4 pos : SV_POSITION;
	float3 tex0 : TEXCOORD0;
};

float4 PS(PixelInput input) : SV_TARGET
{
	float4 color = colorMap.Sample(colorSampler, input.tex0);

	return color;
}

