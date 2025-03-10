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
    #ifdef INSTANCE
        float4 instanceData0 : INSTANCE0;
        float4 instanceData1 : INSTANCE1;
        float4 instanceData2 : INSTANCE2;
    #endif
    };
#else
    Texture2DArray baseColorMap : register(t0);
    Texture2DArray normalMap : register(t1);
    Texture2D idMap : register(t2);
    Texture2D weightMap : register(t3);

    SamplerState baseColorSampler : register(s0);
    SamplerState normalSampler : register(s1);
    SamplerState idSampler : register(s2);
    SamplerState weightSampler : register(s3);

    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float blendRatio;
        float2 idMapTexels;
        float2 weightMapTexels;
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
    float4 worldPosition : WORLD_POS;
#ifdef SHADOW
    float4 shadowPos : SHADOW_POSITION;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        #ifdef INSTANCE
            float4x3 iWorldMatrix = float4x3(
                float3(input.instanceData0.x, input.instanceData1.x, input.instanceData2.x),
                float3(input.instanceData0.y, input.instanceData1.y, input.instanceData2.y),
                float3(input.instanceData0.z, input.instanceData1.z, input.instanceData2.z),
                float3(input.instanceData0.w, input.instanceData1.w, input.instanceData2.w)
                );
        #else
            float4x3 iWorldMatrix = worldMatrix;
        #endif
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, iWorldMatrix);
        float3 worldNormal   = normalize(mul(input.normal, (float3x3)iWorldMatrix));
        float3 worldTangent  = normalize(mul(input.tangent.xyz, (float3x3)iWorldMatrix));
        float3 worldBiNormal = cross(worldTangent, worldNormal) * input.tangent.w;
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position  = clipPosition;
        output.weightTex = input.texcoord;
        output.detailTex = worldPosition.xy / 512.0;
        output.normal   = worldNormal;
        output.tangent  = worldTangent;
        output.biNormal = worldBiNormal;
        output.worldPosition = float4(worldPosition, GetDepth(clipPosition));

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

#define SPLAT_MAP3 1

#if SPLAT_MAP3
    float2 GetUV(float2 uv, float texSize)
    {
        // 纹理有错位偏移一个像素距离
        uv += 1.0 / texSize;
        // 将0~1的uv，映射成1/texSize~2/texSize、4/texSize~5/texSize等多个线性分段
        float texMappingSize = texSize / 3.0;
        float2 mappingUV = floor(uv * texMappingSize) / texMappingSize;
        float2 mappingUVLerp = frac(uv * texMappingSize) / texMappingSize;
        return mappingUV + 1.5 / texSize + (mappingUVLerp - 1.5 / texSize) / 1.5;
    }

    float2 GetIdUV(float2 uv, float texSize)
    {
        return GetUV(uv, texSize);
    }

    float2 GetWeightUV(float2 uv, float texSize)
    {
        return GetUV(uv, texSize);
    }
#else
    float2 GetIdUV(float2 uv, float texSize)
    {
        return uv - 0.5 / texSize;
    }

    float2 GetWeightUV(float2 uv, float texSize)
    {
        uv -= 1.0 / texSize;
        float texMappingSize = texSize * 0.5;
        float2 mappingUV = floor(uv * texMappingSize) / texMappingSize;
        float2 mappingUVLerp = frac(uv * texMappingSize) / texMappingSize;
        return mappingUV + mappingUVLerp * 0.5 + 0.5 / texSize;
    }
#endif

    PixelOutput PS(PixelInput input)
    {
        float3 blendTexIndex = floor(idMap.SampleLevel(idSampler, GetIdUV(input.weightTex, idMapTexels), 0).rgb * 255.0 + 0.5);
        float3 weight = weightMap.SampleLevel(weightSampler, GetWeightUV(input.weightTex, weightMapTexels), 0).rgb;

        float4 texDiff1 = baseColorMap.Sample(baseColorSampler, float3(input.detailTex, blendTexIndex.x));
        float4 texDiff2 = baseColorMap.Sample(baseColorSampler, float3(input.detailTex, blendTexIndex.y));
        float4 texDiff3 = baseColorMap.Sample(baseColorSampler, float3(input.detailTex, blendTexIndex.z));

        float4 texNormal1 = normalMap.Sample(normalSampler, float3(input.detailTex, blendTexIndex.x));
        float4 texNormal2 = normalMap.Sample(normalSampler, float3(input.detailTex, blendTexIndex.y));
        float4 texNormal3 = normalMap.Sample(normalSampler, float3(input.detailTex, blendTexIndex.z));

    #if SPLAT_MAP3
        weight = GetBlendThreeFactor(float3(texDiff1.a, texDiff2.a, texDiff3.a), weight);
    #endif

        // => xyz - color, w - metallic
        texDiff1.a = texNormal1.a;
        texDiff2.a = texNormal2.a;
        texDiff3.a = texNormal3.a;
        texDiff1 = GammaToLinearSpace(texDiff1);
        texDiff2 = GammaToLinearSpace(texDiff2);
        texDiff3 = GammaToLinearSpace(texDiff3);

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

        PBRContext context;
        context.diffuseColor = baseColorMetallic.rgb;
        context.metallic = baseColorMetallic.a;
        context.roughness = normalRoughness.w;
        context.specular = 0.5;
        context.emissiveColor = float3(0.0, 0.0, 0.0);
        context.worldPosition = input.worldPosition.xyz;
        context.normalDirection = normalize(normalRoughness.xyz);
        context.tangentDirection = input.tangent;
        context.bnormalDirection = input.biNormal;
        context.viewDirection = normalize(cameraPos - input.worldPosition.xyz);

    #ifdef SHADOW
        context.shadow = GetShadow(input.shadowPos, input.worldPosition.w);
    #else
        context.shadow = 1.0;
    #endif
        context.occlusion = 1.0;
        context.alpha = 1.0;

    #if 0
        context.diffuseColor = float3(0, 0, 0);
        if (blendTexIndex.x == 1)
            context.diffuseColor.r = weight.r;
        if (blendTexIndex.y == 1)
            context.diffuseColor.r = weight.g;
        if (blendTexIndex.z == 1)
            context.diffuseColor.r = weight.b;

        // if (blendTexIndex.x == 2)
        //     context.diffuseColor.g = weight.r;
        // if (blendTexIndex.y == 2)
        //     context.diffuseColor.g = weight.g;
        // if (blendTexIndex.z == 2)
        //     context.diffuseColor.g = weight.b;
    #endif

        return PBRPipline(context);
    }
#endif
