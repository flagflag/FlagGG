#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 pos : POSITION;
        float2 tex : TEXCOORD;
        float3 nor : NORMAL;
    };
#else
    Texture2D weightMap : register(t0);
    Texture2D detail1Map : register(t1);
    Texture2D detail2Map : register(t2);
    Texture2D detail3Map : register(t3);

    SamplerState weightSampler : register(s0);
    SamplerState detail1Sampler : register(s1);
    SamplerState detail2Sampler : register(s2);
    SamplerState detail3Sampler : register(s3);

    #ifdef SHADOW
        Texture2D shadowMap : register(t6);
        SamplerState shadowSampler : register(s6);
    #endif


#endif

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 weightTex : TEXCOORD;
    float2 detailTex : TEXCOORD1;
	float3 nor : NORMAL;
#ifdef SHADOW
    float shadowDepth : DEPTH;
    float4 shadowScreenPos : SCREENPOS;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.pos.w = 1.0;
        float3 worldPos = mul(input.pos, worldMatrix);
        float3 worldNor = normalize(mul(input.nor, (float3x3)worldMatrix));
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.pos = clipPos;
        output.weightTex = input.tex;
        output.detailTex = float2(32.0, 32.0) * output.weightTex;
        output.nor = worldNor;

    #ifdef SHADOW
        float4 shadowClipPos = mul(float4(worldPos, 1.0), lightProjviewMatrix);
        output.shadowDepth = GetDepth(shadowClipPos);
        output.shadowScreenPos = GetScreenPos(shadowClipPos);
    #endif

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float3 weight = Sample2D(weight, input.weightTex).xyz;
        float sumWeight = weight.x + weight.y + weight.z;
        weight /= sumWeight;
        float4 diffColor = weight.x * Sample2D(detail1, input.detailTex) +
                            weight.y * Sample2D(detail2, input.detailTex) +
                            weight.z * Sample2D(detail3, input.detailTex);

        float4 matDiffColor = float4(0.5, 0.5, 0.5, 16);
        float4 color = matDiffColor * diffColor;

    #ifdef SHADOW
        float2 shadowTex = input.shadowScreenPos.xy / input.shadowScreenPos.w;
        // 接下来几步做法是用step代替if...else...来做到硬件加速
        float t1 = step(0.0, saturate(shadowTex.x) - shadowTex.x) * step(0.0, shadowTex.x - saturate(shadowTex.x));
        float t2 = step(0.0, saturate(shadowTex.y) - shadowTex.y) * step(0.0, shadowTex.y - saturate(shadowTex.y));
        
        float shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex).xy);
        float s1 = step(0.0, input.shadowDepth - shadowDepth);
        float final = t1 * t2 * s1;

        float pixelOffset = 0.002;

        shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex + float2(-pixelOffset, 0)).xy);
        float s2 = step(0.0, input.shadowDepth - shadowDepth);
        final += t1 * t2 * s2;

        shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex + float2(pixelOffset, 0)).xy);
        float s3 = step(0.0, input.shadowDepth - shadowDepth);
        final += t1 * t2 * s3;

        shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex + float2(0, -pixelOffset)).xy);
        float s4 = step(0.0, input.shadowDepth - shadowDepth);
        final += t1 * t2 * s4;

        shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex + float2(0, pixelOffset)).xy);
        float s5 = step(0.0, input.shadowDepth - shadowDepth);
        final += t1 * t2 * s5;

        final *= 0.2;

        color = float4(color.xyz * (1.0 - final), 1.0);
    #endif

        return color;
    }
#endif
