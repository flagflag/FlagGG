
#ifdef MODEL_SHADOW
struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
	float3 worldPos : WORLD_POS;
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

float4 EncodeFloatRGBA(float value)
{
	float4 kEncodeMul = float4(1.0, 255.0, 65025.0, 16581375.0);
    float kEncodeBit = 1.0 / 255.0;
    float4 enc = kEncodeMul * value;
    enc = frac(enc);
    enc -= enc.yzww * kEncodeBit;
    return enc;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float depth = input.pos.z / input.pos.w;
	return EncodeFloatRGBA(depth);
}

