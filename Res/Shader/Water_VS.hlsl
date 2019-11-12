cbuffer MatrixBuffer : register(b0)
{
	float4x3 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

struct VertexInput
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
	float4 screenPos : SCREEN_POS;
};

PixelInput VS(VertexInput input)
{
	input.pos.w = 1.0;
	float3 worldPos = mul(input.pos, worldMatrix);
	float3 worldNor = normalize(mul(input.nor, (float3x3)worldMatrix));
	float4 clipPos = mul(float4(worldPos, 1.0), viewMatrix);
	
	PixelInput output;
	output.pos = mul(clipPos, projectionMatrix);
	output.tex0 = input.tex0;
	output.nor = worldNor;
	output.screenPos = output.pos;

	return output;
}
