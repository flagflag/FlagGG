Texture2D colorMap_;
SamplerState colorSampler_;
 
struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};
 
float4 PS( PixelInput input ) : SV_TARGET
{
	return colorMap_.Sample( colorSampler_, input.tex0 );
}

