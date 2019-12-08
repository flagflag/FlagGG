cbuffer MatrixBuffer : register(b0)
{
	float4x3 worldMatrix;
	float4x3 viewMatrix;
	float4x4 projviewMatrix;
}

#ifdef SKINNED
cbuffer SkinMatrixBuffer : register(b1)
{
	uniform float4x3 skinMatrices[64];
}
#endif

cbuffer ParamBuffer : register(b2)
{
	float deltaTime;
	float elapsedTime;
	float3 cameraPos;
	float3 lightPos;
	float3 lightDir;
	float4x3 lightViewMatrix;
	float4x4 lightProjviewMatrix;
}

struct VertexInput
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
#ifdef COLOR
	float4 color : COLOR;
#endif
#ifdef SKINNED
	float4 blendWeights : BLEND_WEIGHTS;
	int4 blendIndices : BLEND_INDICES;
#endif
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
#ifdef COLOR
	float4 color : COLOR;
#endif
	float3 worldPos : WORLD_POS;
#ifdef SHADOW
	float4 shadowPos : POSITION;
#endif
};

#ifdef SKINNED
float4x3 GetSkinMatrix(float4 blendWeights, int4 blendIndices)
{
	return 
	skinMatrices[blendIndices.x] * blendWeights.x +
	skinMatrices[blendIndices.y] * blendWeights.y +
	skinMatrices[blendIndices.z] * blendWeights.z +
	skinMatrices[blendIndices.w] * blendWeights.w;
}
#endif

PixelInput VS(VertexInput input)
{
#ifdef STATIC
	float4x3 iWorldMatrix = worldMatrix;
#else
	float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
#endif
	input.pos.w = 1.0;
	float3 worldPos = mul(input.pos, iWorldMatrix);
	float3 worldNor = normalize(mul(input.nor, (float3x3)iWorldMatrix));
	float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
	
	PixelInput output;
	output.pos = clipPos;
	output.tex0 = input.tex0;
	output.nor = worldNor;
#ifdef COLOR
	output.color = input.color;
#endif
	output.worldPos = worldPos;

#ifdef SHADOW
	float4 shadowClipPos = mul(float4(worldPos, 1.0), lightProjviewMatrix);
	output.shadowPos = shadowClipPos;
#endif

	return output;
}

