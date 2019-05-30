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
	float4 pos : POSITION;
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

float4x3 GetSkinMatrix(float4 blendWeights, int4 blendIndices)
{
	return 
	skinMatrices[blendIndices.x] * blendWeights.x +
	skinMatrices[blendIndices.y] * blendWeights.y +
	skinMatrices[blendIndices.z] * blendWeights.z +
	skinMatrices[blendIndices.w] * blendWeights.w;
}

PixelInput VS(VertexInput input)
{
	float4x3 skinWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
	float3 worldPos = mul(input.pos, skinWorldMatrix);
	float3 worldNor = normalize(mul(input.nor, (float3x3)skinWorldMatrix));
	float4 clipPos = mul(float4(worldPos, 1.0), viewMatrix);
	
	PixelInput output;
	output.pos = mul(clipPos, projectionMatrix);
	output.tex0 = input.tex0;
	output.nor = worldNor;

	return output;
}

