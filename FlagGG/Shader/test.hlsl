Texture2D colorMap_ : register( t0 );
 
SamplerState colorSampler_ : register( s0 );
 
struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};
 
struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};
 
PS_Input VS( VS_Input vertex )
{
	PS_Input vsOut = ( PS_Input )0;
	 
	vsOut.pos = vertex.pos;
	 
	vsOut.tex0 = vertex.tex0;

	vsOut.nor = vertex.nor;
	 
	return vsOut;
}
 
float4 PS( PS_Input frag ) : SV_TARGET
{
	return colorMap_.Sample( colorSampler_, frag.tex0 );
}
 
 
 