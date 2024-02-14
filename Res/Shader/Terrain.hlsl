#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"
#include "Shader/Lighting.hlsl"
#include "Shader/GI/Ambient.hlsl"
#include "Shader/PBR/PBR.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 position : POSITION;
        float2 texcoord : TEXCOORD;
        float3 normal   : NORMAL;
        float4 tangent  : TANGENT;
    };
#else
    Texture2DArray baseColorMap : register(t0);
    Texture2DArray normalMap : register(t1);
    Texture2D weight1Map : register(t2);
    Texture2D weight2Map : register(t3);

    SamplerState baseColorSampler : register(s0);
    SamplerState normalSampler : register(s1);
    SamplerState weight1Sampler : register(s2);
    SamplerState weight2Sampler : register(s3);

    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float3 blendTexIndex;
    }
#endif

struct PixelInput
{
	float4 position  : SV_POSITION;
	float2 weightTex : TEXCOORD;
    float2 detailTex : TEXCOORD1;
	float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 biNormal  : BINORMAL;
    float3 worldPosition : WORLD_POS;
#ifdef SHADOW
    float4 shadowPos : POSITION;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, worldMatrix);
        float3 worldNormal   = normalize(mul(input.normal, (float3x3)worldMatrix));
        float3 worldTangent  = normalize(mul(input.tangent.xyz, (float3x3)worldMatrix));
        float3 worldBiNormal = cross(worldTangent, worldNormal) * input.tangent.w;
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position  = clipPosition;
        output.weightTex = input.texcoord;
        output.detailTex = worldPosition.xy / 2.56;
        output.normal   = worldNormal;
        output.tangent  = worldTangent;
        output.biNormal = worldBiNormal;
        output.worldPosition = worldPosition;

    #ifdef SHADOW
        output.shadowPos = GetShadowPos(worldPosition);
    #endif

        return output;
    }
#else
    float3 GetBlendThreeFactor(float3 height, float3 weight)
    {
        height = max(height, float3(0.0001, 0.0001, 0.0001));
        float3 hw = height * weight;
        float maxHW = max(max(hw.x, hw.y), hw.z) * 0.3;
        float3 ww = max(hw - maxHW, float3(0.0, 0.0, 0.0)) * weight;
        return ww / (ww.x + ww.y + ww.z);
    }

    float4 PS(PixelInput input) : SV_TARGET
    {
    #ifdef SHADOW
        float shadow = GetShadow(input.shadowPos);
    #else
        float shadow = 1.0;
    #endif

        float T1 = Sample2D(weight1, input.weightTex).r;
        float T2 = Sample2D(weight2, input.weightTex).r;
        float3 weight = float3((1.0 - T1) * (1.0 - T2), T1 * (1.0 - T2), T2);

        float4 texDiff1 = baseColorMap.Sample(baseColorSampler, float3(input.detailTex, blendTexIndex.x));
        float4 texDiff2 = baseColorMap.Sample(baseColorSampler, float3(input.detailTex, blendTexIndex.y));
        float4 texDiff3 = baseColorMap.Sample(baseColorSampler, float3(input.detailTex, blendTexIndex.z));

        weight = GetBlendThreeFactor(float3(texDiff1.a, texDiff2.a, texDiff3.a), weight);

        float4 texNormal1 = normalMap.Sample(normalSampler, float3(input.detailTex, blendTexIndex.x));
        float4 texNormal2 = normalMap.Sample(normalSampler, float3(input.detailTex, blendTexIndex.y));
        float4 texNormal3 = normalMap.Sample(normalSampler, float3(input.detailTex, blendTexIndex.z));

        // => xyz - color, w - metallic
        texDiff1.a = texNormal1.a;
        texDiff2.a = texNormal2.a;
        texDiff3.a = texNormal3.a;

        // decode normal => xyz - normal, w - roughness
        texNormal1 = float4(DecodeNormal(texNormal1.xyyy), texNormal1.b);
        texNormal2 = float4(DecodeNormal(texNormal2.xyyy), texNormal2.b);
        texNormal3 = float4(DecodeNormal(texNormal3.xyyy), texNormal3.b);

        float4 baseColorMetallic = weight.x * texDiff1   + weight.y * texDiff2   + weight.z * texDiff3;
        float4 normalRoughness   = weight.x * texNormal1 + weight.y * texNormal2 + weight.z * texNormal3;

        float3 row0 = float3(input.tangent.x, input.biNormal.x, input.normal.x);
        float3 row1 = float3(input.tangent.y, input.biNormal.y, input.normal.y);
        float3 row2 = float3(input.tangent.z, input.biNormal.z, input.normal.z);
        normalRoughness.xyz = float3(dot(row0, normalRoughness.xyz), dot(row1, normalRoughness.xyz), dot(row2, normalRoughness.xyz));

        float3 viewDirection = normalize(cameraPos - input.worldPosition);
        float3 color = PBR_BRDF(baseColorMetallic.rgb, baseColorMetallic.a, normalRoughness.w, input.worldPosition, normalRoughness.xyz, viewDirection, shadow, 1.0);

        return float4(LinearToGammaSpace(color), 1.0);
    }
#endif
