Texture2D colorMap_ : register(t0);
SamplerState colorSampler_ : register(s0);
 
struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};

float4 PS( PixelInput input ) : SV_TARGET
{
	float4 textureColor = colorMap_.Sample( colorSampler_, input.tex0 );

	float4 diffuseColor = { 1.0, 1.0, 1.0, 1.0 };
	float3 lightDir = { 1.0, 0, 1.0 };

	float lightIntensity = saturate(dot(input.nor, lightDir));
	float4 color = saturate(diffuseColor * lightIntensity);

	color = color * textureColor;

	return color;
}

