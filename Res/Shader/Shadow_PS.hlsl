
#ifdef MODEL_SHADOW
struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};
#elif TERRAIN_SHADOW
struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 weightTex : TEXCOORD0;
    float2 detailTex : TEXCOORD1;
	float3 nor : NORMAL;
};
#endif

float4 PS(PixelInput input) : SV_TARGET
{
	float depth = input.pos.z / input.pos.w;
	return float4(depth, depth, depth, 1);
}

