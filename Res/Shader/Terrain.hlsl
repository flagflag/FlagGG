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
	float4 shadowPos : POSITION;
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
        output.shadowPos = shadowClipPos;
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
        float bias = 0.001;
        float2 shadowTex;
        shadowTex.x = input.shadowPos.x / input.shadowPos.w * 0.5 + 0.5;
        shadowTex.y = input.shadowPos.y / input.shadowPos.w * (-0.5) + 0.5;
        if (saturate(shadowTex.x) == shadowTex.x && saturate(shadowTex.y) == shadowTex.y)
        {
            float shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex).xy)  + bias;
            float depth = input.shadowPos.z / input.shadowPos.w;
            if (shadowDepth < depth)
            {
                color = float4(0, 0, 0, 1);
            }
        }
    #endif

        return color;
    }
#endif
