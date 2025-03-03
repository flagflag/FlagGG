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
        float blendRatio;
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
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, worldMatrix);
        float3 worldNormal   = normalize(mul(input.normal, (float3x3)worldMatrix));
        float3 worldTangent  = normalize(mul(input.tangent.xyz, (float3x3)worldMatrix));
        float3 worldBiNormal = cross(worldTangent, worldNormal) * input.tangent.w;
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position  = clipPosition;
        output.weightTex = input.texcoord;
        output.detailTex = worldPosition.xy / 10.24;
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

    float4 Hash4(float2 p)
    {
        return frac(sin(float4( 1.0+dot(p,float2(37.0,17.0)), 
                                              2.0+dot(p,float2(11.0,47.0)),
                                              3.0+dot(p,float2(41.0,29.0)),
                                              4.0+dot(p,float2(23.0,31.0))))*103.0);
    }

    #define USE_HASH 1
    float4 SampleNoTiling(Texture2DArray tex, SamplerState sam, float2 uv, float layer)
    {
        float2 iuv = floor(uv);
        float2 fuv = frac(uv);

        // Generate per-tile transformation
        #if defined (USE_HASH)
            float4 ofa = Hash4(iuv + float2(0, 0));
            float4 ofb = Hash4(iuv + float2(1, 0));
            float4 ofc = Hash4(iuv + float2(0, 1));
            float4 ofd = Hash4(iuv + float2(1, 1));
        #else
            float4 ofa = noiseMap.Sample(noiseSampler, (iuv + float2(0.5, 0.5)) / 256.0);
            float4 ofb = noiseMap.Sample(noiseSampler, (iuv + float2(1.5, 0.5)) / 256.0);
            float4 ofc = noiseMap.Sample(noiseSampler, (iuv + float2(0.5, 1.5)) / 256.0);
            float4 ofd = noiseMap.Sample(noiseSampler, (iuv + float2(1.5, 1.5)) / 256.0);
        #endif

        // Compute the correct derivatives
        float2 dx = ddx(uv);
        float2 dy = ddy(uv);

        // Mirror per-tile uvs
        ofa.zw = sign(ofa.zw - 0.5);
        ofb.zw = sign(ofb.zw - 0.5);
        ofc.zw = sign(ofc.zw - 0.5);
        ofd.zw = sign(ofd.zw - 0.5);

        float2 uva = uv * ofa.zw + ofa.xy, dxa = dx * ofa.zw, dya = dy * ofa.zw;
        float2 uvb = uv * ofb.zw + ofb.xy, dxb = dx * ofb.zw, dyb = dy * ofb.zw;
        float2 uvc = uv * ofc.zw + ofc.xy, dxc = dx * ofc.zw, dyc = dy * ofc.zw;
        float2 uvd = uv * ofd.zw + ofd.xy, dxd = dx * ofd.zw, dyd = dy * ofd.zw;

        // Fetch and blend
        float2 b = smoothstep(blendRatio, 1.0 - blendRatio, fuv);

        return lerp(lerp(tex.SampleGrad(sam, float3(uva, layer), dxa, dya), tex.SampleGrad(sam, float3(uvb, layer), dxb, dyb), b.x),
                    lerp(tex.SampleGrad(sam, float3(uvc, layer), dxc, dyc), tex.SampleGrad(sam, float3(uvd, layer), dxd, dyd), b.x), b.y);
    }

    PixelOutput PS(PixelInput input)
    {
        float T1 = Sample2D(weight1, input.weightTex).r;
        float T2 = Sample2D(weight2, input.weightTex).r;
        float3 weight = float3((1.0 - T1) * (1.0 - T2), T1 * (1.0 - T2), T2);

        float4 texDiff1 = SampleNoTiling(baseColorMap, baseColorSampler, input.detailTex, blendTexIndex.x);
        float4 texDiff2 = SampleNoTiling(baseColorMap, baseColorSampler, input.detailTex, blendTexIndex.y);
        float4 texDiff3 = SampleNoTiling(baseColorMap, baseColorSampler, input.detailTex, blendTexIndex.z);

        weight = GetBlendThreeFactor(float3(texDiff1.a, texDiff2.a, texDiff3.a), weight);

        float4 texNormal1 = SampleNoTiling(normalMap, normalSampler, input.detailTex, blendTexIndex.x);
        float4 texNormal2 = SampleNoTiling(normalMap, normalSampler, input.detailTex, blendTexIndex.y);
        float4 texNormal3 = SampleNoTiling(normalMap, normalSampler, input.detailTex, blendTexIndex.z);

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

        return PBRPipline(context);
    }
#endif
