Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);
#ifdef SHADOW
Texture2D shadowMap : register(t6);
SamplerState shadowSampler : register(s6);
#endif

cbuffer ParamBuffer : register(b2)
{
	float deltaTime;
	float elapsedTime;
	float3 cameraPos;
	float3 lightPos;
	float3 lightDir;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
#ifdef SHADOW
	float4 shadowPos : POSITION;
#endif
};

float4 PS(PixelInput input) : SV_TARGET
{
	float4 textureColor = colorMap.Sample(colorSampler, input.tex0);

	float4 diffuseColor = { 1.0, 1.0, 1.0, 1.0 };

	float lightIntensity = saturate(dot(input.nor, -lightDir));
	float4 color = saturate(diffuseColor * lightIntensity);

	color = color * textureColor;

#ifdef SHADOW
	float bias = 0.001;
	float2 shadowTex;
	shadowTex.x = input.shadowPos.x / input.shadowPos.w * 0.5 + 0.5;
	shadowTex.y = input.shadowPos.y / input.shadowPos.w * (-0.5) + 0.5;
	if (saturate(shadowTex.x) == shadowTex.x && saturate(shadowTex.y) == shadowTex.y)
	{
		float shadowDepth = shadowMap.Sample(shadowSampler, shadowTex).x + bias;
		float depth = input.shadowPos.z / input.shadowPos.w;
		if (shadowDepth < depth)
		{
			color = float4(1, 1, 1, 1);
		}
	}
#endif

	return color;
}

