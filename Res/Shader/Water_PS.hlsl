Texture2D noiseMap : register(t0);
SamplerState noiseSampler : register(s0);

Texture2D envMap : register(t5);
SamplerState envSampler : register(s5);

cbuffer ParamBuffer : register(b0)
{
	float deltaTime;
	float elapsedTime;
	float3 lightPos;
	float3 lightDir;
	float3 cameraPos;

	// 材质参数
	float noiseTiling;
	float2 noiseSpeed;
	float noiseStrength;
}

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
	float3 worldPos : WORLD_POS;
};

float4 PS(PixelInput input) : SV_TARGET
{
	// float4 color = float4(1, 1, 1, 1);
	// float2 envTex;
	// envTex.x = input.pos.x / input.pos.w * 0.5 + 0.5;
	// envTex.y = input.pos.y / input.pos.w * (-0.5) + 0.5;
	// color = envMap.Sample(envSampler, envTex);
	float2 tex = input.tex0 * noiseTiling * noiseSpeed * elapsedTime;
	float2 noise = (noiseMap.Sample(noiseSampler, tex).rg - 0.5) * noiseStrength;
	
    return color;
}
