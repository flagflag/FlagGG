cbuffer MatrixBuffer : register(b0)
{
	float4x3 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

struct VertexInput
{
	float4 pos : POSITION;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float3 tex0 : TEXCOORD0;
};

PixelInput VS(VertexInput input)
{
	input.pos.w = 1.0;
	float3 worldPos = mul(input.pos, worldMatrix);
	float4 clipPos = mul(float4(worldPos, 1.0), viewMatrix);
	
	PixelInput output;
	output.pos = mul(clipPos, projectionMatrix);
	output.tex0 = input.pos.xyz;

	return output;
}

