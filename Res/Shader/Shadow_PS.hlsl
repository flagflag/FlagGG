
struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};

float4 PS(PixelInput input) : SV_TARGET
{
	return 1.0;
}

