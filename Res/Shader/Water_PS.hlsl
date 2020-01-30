Texture2D noiseMap : register(t0);
SamplerState noiseSampler : register(s0);

Texture2D diffMap : register(t1);
SamplerState diffSampler : register(s1);

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
	float3 waterColor;
	float fresnelPower;
}

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
	float4 screenPos : SCREENPOS;
	float2 reflectTex : TEXCOORD1;
	float4 eyeVec : EYEVEC;
};

float4 PS(PixelInput input) : SV_TARGET
{
	float4 color = float4(1, 1, 1, 1);

	float2 refractTex = input.screenPos.xy / input.screenPos.w;
	float2 reflectTex = input.reflectTex.xy / input.screenPos.w;

	float2 tex = input.tex * noiseTiling * noiseSpeed * elapsedTime;
	float2 noise = (noiseMap.Sample(noiseSampler, tex).rg - 0.5) * noiseStrength;
	
	refractTex += noise;
	if (noise.y < 0.0)
        noise.y = 0.0;
	reflectTex += noise;

	float3 refractColor = envMap.Sample(envSampler, refractTex).rgb * waterColor;
	float3 reflectColor = diffMap.Sample(diffSampler, reflectTex).rgb;

	float fresnel = pow(1.0 - saturate(dot(normalize(input.eyeVec.xyz), input.nor)), fresnelPower);

	color = float4(lerp(refractColor, reflectColor, fresnel), 1.0);
	
    return color;
}
