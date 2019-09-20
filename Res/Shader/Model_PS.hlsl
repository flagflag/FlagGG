Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);
#ifdef SHADOW
Texture2D shadowMap : register(t6);
SamplerState shadowSampler : register(s6);
#endif

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
	float3 lightDir = { -1.0, -1.0, 1.0 };
	lightDir = -lightDir;

	float lightIntensity = saturate(dot(input.nor, lightDir));
	float4 color = saturate(diffuseColor * lightIntensity);

	color = color * textureColor;

#ifdef SHADOW
	float bias = 0.001;
	float2 shadowTex = float2(shadowPos.x / shadowPos.z, shadowPos.y / shadowPos.z);
	float shadowDepth = shadowMap.Sample(shadowSampler, shadowTex).x + bias;
	float depth = pos.z / pos.w;
	if (shadowDepth < depth)
	{
		color = 1.0;
	}
#endif

	return color;
}

