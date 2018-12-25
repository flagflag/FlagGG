struct VertexInput
{
	float3 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};
 
PixelInput VS( VertexInput input )
{
	PixelInput output = (PixelInput)0;

	output.pos.xzy = input.pos.xzy;
	output.pos.w = 1;
	output.tex0 = input.tex0;
	output.nor = input.nor;

	return output;
}

