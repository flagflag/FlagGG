cbuffer MatrixBuffer : register(c0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

cbuffer SkinMatrixBuffer : register(c1)
{
	float4x3 skinMatrices[64];
}

struct VertexInput
{
	float3 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
	float4 blendWeights : BLEND_WEIGHTS;
	int4 blendIndices : BLEND_INDICES;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};

float4x3 GetSkinMatrix(float4 blendWeights, float4 blendIndices)
{
	return 
	skinMatrices[blendIndices.x] * blendWeights.x +
	skinMatrices[blendIndices.y] * blendWeights.y +
	skinMatrices[blendIndices.z] * blendWeights.z +
	skinMatrices[blendIndices.w] * blendWeights.w;
}

PixelInput VS(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	output.pos.xzy = input.pos.xzy;
	output.pos.w = 1.0;
	output.tex0 = input.tex0;
	output.nor = mul(input.nor, (float3x3)worldMatrix);
	output.nor = normalize(output.nor);

	// output.pos = mul(output.pos, worldMatrix);
	float4x3 skinWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
	output.pos = float4(mul(output.pos, skinWorldMatrix), 1.0);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	return output;
}

