#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"
#include "Shader/Lighting.hlsl"
#include "Shader/PBS/HairSimulationBRDF.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float3 position : POSITION;
        float2 texcoord : TEXCOORD;
        float3 normal   : NORMAL;
        float4 tangent  : TANGENT;
    };
#else
    Texture2D hairSpecShiftMap : register(t0);
    SamplerState hairSpecShiftSampler : register(s0);
    Texture2D hairAlpha : register(t1);
    SamplerState hairAlphaSampler : register(s1);
    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float hairTileX;
        float hairTileY;
        float distorsion;
        float hairOffset;
        float hairWidth;
        float hairSpecular;
        float3 baseColorA;
        float3 baseColorB;
    }
#endif

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float3 biNormal : BINORMAL;
	float4 worldPosition : WORLDPOS;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        float4x3 iWorldMatrix = worldMatrix;
        float3 worldPosition = mul(float4(input.position, 1.0), iWorldMatrix);
        float3 worldNormal   = normalize(mul(input.normal, (float3x3)iWorldMatrix));
        float3 worldTangent  = normalize(mul(input.tangent.xyz, (float3x3)iWorldMatrix));
        float3 worldBiNormal = cross(worldTangent, worldNormal) * input.tangent.w;
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPosition;
        output.texcoord = input.texcoord;
        output.normal   = worldNormal;
        output.tangent  = worldTangent;
        output.biNormal = worldBiNormal;
        output.worldPosition = float4(worldPosition, GetDepth(clipPosition));

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        HairContext context;
        float shift = GammaToLinearSpace(hairSpecShiftMap.Sample(hairSpecShiftSampler, float2(hairTileX, hairTileY) * input.texcoord)).r;
        float alpha = hairAlpha.Sample(hairAlphaSampler, input.texcoord).r;
        context.diffuseColor = lerp(baseColorA, baseColorB, shift * shift);
        context.shift = shift * distorsion + hairOffset;
        context.hairWidth = hairWidth;
        context.hairSpecular = hairSpecular;
        context.viewDirection = normalize(cameraPos - input.worldPosition.xyz);
        context.normalDirection = normalize(input.normal);
        context.tangentDirection = input.tangent;
        context.bnormalDirection = input.biNormal;
        return float4(HairSimulationBRDF(context), alpha);
    }
#endif
