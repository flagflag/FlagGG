#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"

// #define SHADOW_CMP 1

#ifdef VERTEX
    struct VertexInput
    {
        float4 position : POSITION;
        float2 texcoord : TEXCOORD;
        float3 normal   : NORMAL;
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
#endif

struct PixelInput
{
	float4 position  : SV_POSITION;
	float2 weightTex : TEXCOORD;
    float2 detailTex : TEXCOORD1;
	float3 normal : NORMAL;
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
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position  = clipPosition;
        output.weightTex = input.texcoord;
        output.detailTex = float2(32.0, 32.0) * output.weightTex;
        output.normal    = worldNormal;

    #ifdef SHADOW
        output.shadowPos = GetShadowPos(worldPosition);
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
        float shadow = GetShadow(input.shadowPos);
    #else
        float shadow = 1.0;
    #endif

        return LinearToGammaSpace(color * shadow);
    }
#endif
