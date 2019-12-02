Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);
#ifdef DISSOLVE
Texture2D noiseMap : register(t1);
SamplerState noiseSampler : register(s1);
#endif
#ifdef SHADOW
Texture2D shadowMap : register(t6);
SamplerState shadowSampler : register(s6);
#endif

cbuffer ParamBuffer : register(b0)
{
	float deltaTime;
	float elapsedTime;
	float3 lightPos;
	float3 lightDir;
	float3 cameraPos;

	// 材质参数
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float emissivePower;
	float dissolveTime;
}

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
#ifdef COLOR
	float4 color : COLOR;
#endif
	float3 worldPos : WORLD_POS;
#ifdef SHADOW
	float4 shadowPos : POSITION;
#endif
};

float DecodeFloatRG(float2 enc)
{  
    float2 kDecodeDot = float2(1.0, 1.0 / 255.0);
    return dot(enc, kDecodeDot);
}

float4 PS(PixelInput input) : SV_TARGET
{
	float lightIntensity = saturate(dot(input.nor, -lightDir));
	float3 diffColor = saturate(diffuseColor.rgb * lightIntensity);
#ifndef COLOR
	float3 textureColor = colorMap.Sample(colorSampler, input.tex0).rgb;
#else
	float3 textureColor = input.color.rgb * input.color.a;
#endif
	diffColor = diffColor * textureColor;

	float3 reflectDir = (input.nor * -lightDir * 2.0) * input.nor - (-lightDir);
	float3 eyeDir = cameraPos - input.worldPos;
	float3 specColor = specularColor.rgb * pow(saturate(dot(eyeDir, reflectDir)), emissivePower);

#ifdef SHADOW
	float bias = 0.001;
	float2 shadowTex;
	shadowTex.x = input.shadowPos.x / input.shadowPos.w * 0.5 + 0.5;
	shadowTex.y = input.shadowPos.y / input.shadowPos.w * (-0.5) + 0.5;
	if (saturate(shadowTex.x) == shadowTex.x && saturate(shadowTex.y) == shadowTex.y)
	{
		float shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex).xy) + bias;
		float depth = input.shadowPos.z / input.shadowPos.w;
		if (shadowDepth < depth)
		{
			// diffColor = float3(0, 0, 0);
		}
	}
#endif

	float ambiPower = ambientColor.a / 255.0;
	float diffPower = diffuseColor.a / 255.0;
	float specPower = specularColor.a / 255.0;
	float4 color = float4(ambientColor.rgb * ambiPower + diffColor * diffPower + specColor * specPower, 1.0);

// 溶解特效
#ifdef DISSOLVE
	float dissolveNoise = noiseMap.Sample(noiseSampler, input.tex0).r;
	float temp = dissolveNoise + dissolveTime;
	if (temp > 1)
		discard;
	if (temp > 0.9)
		color += float4(0.94, 0.9, 0.54, 0);
#endif

	return color;
}

