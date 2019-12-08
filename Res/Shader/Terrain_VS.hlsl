cbuffer MatrixBuffer : register(b0)
{
	float4x3 worldMatrix;
	float4x3 viewMatrix;
	float4x4 projviewMatrix;
}

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
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 weightTex : TEXCOORD0;
    float2 detailTex : TEXCOORD1;
	float3 nor : NORMAL;
#ifdef SHADOW
	float4 shadowPos : POSITION;
#endif
};

PixelInput VS(VertexInput input)
{
	input.pos.w = 1.0;
	float3 worldPos = mul(input.pos, worldMatrix);
	float3 worldNor = normalize(mul(input.nor, (float3x3)worldMatrix));
	float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
	
	PixelInput output;
	output.pos = clipPos;
	output.weightTex = input.tex0;
    output.detailTex = float2(32.0, 32.0) * output.weightTex;
	output.nor = worldNor;

#ifdef SHADOW
	float4 shadowClipPos = mul(float4(worldPos, 1.0), lightProjviewMatrix);
	output.shadowPos = shadowClipPos;
#endif

	return output;
}

