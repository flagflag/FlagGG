cbuffer MatrixBuffer : register(b0)
{
	float4x3 worldMatrix;
	float4x3 viewMatrix;
	float4x4 projviewMatrix;
}

cbuffer ParamBuffer : register(b2)
{
	float3 cameraPos;
}

struct VertexInput
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
	float4 screenPos : SCREENPOS;
	float2 reflectTex : TEXCOORD1;
	float4 eyeVec : EYEVEC;
};

float4 GetScreenPos(float4 clipPos)
{
	return float4(
		clipPos.x * 0.5 + clipPos.w * 0.5,
		-clipPos.y * 0.5 + clipPos.w * 0.5,
		0.0,
		clipPos.w
	);
}

float2 GetQuadTexCoord(float4 clipPos)
{
	return float2(
		clipPos.x / clipPos.w * 0.5 + 0.5,
		-clipPos.y / clipPos.w * 0.5 + 0.5
	);
}

float GetDepth(float4 clipPos)
{
    return dot(clipPos.zw, float2(0.0, 1.0 / 1000000000.0));
}

PixelInput VS(VertexInput input)
{
	input.pos.w = 1.0;
	float3 worldPos = mul(input.pos, worldMatrix);
	float3 worldNor = normalize(mul(input.nor, (float3x3)worldMatrix));
	float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
	
	PixelInput output;
	output.pos = clipPos;
	output.tex = input.tex;
	output.nor = worldNor;
	output.screenPos = GetScreenPos(clipPos);
	output.reflectTex = GetQuadTexCoord(clipPos) * clipPos.w;
	output.eyeVec = float4(cameraPos - worldPos, GetDepth(clipPos));

	return output;
}
