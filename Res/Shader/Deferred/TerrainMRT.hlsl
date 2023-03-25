#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

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
	float3 normal    : NORMAL;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.position.w = 1.0;
        float3 worldPos = mul(input.position, worldMatrix);
        float3 worldNor = normalize(mul(input.normal, (float3x3)worldMatrix));
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPos;
        output.weightTex = input.texcoord;
        output.detailTex = float2(32.0, 32.0) * output.weightTex;
        output.normal = worldNor;

        return output;
    }
#else
    struct PixelOutput
    {
        // rgb - normal
	    // a   - ao
        float4 GBufferA : SV_Target0;
        // r - metallic
        // g - specular
        // b - roughness
        // a - 
        float4 GBufferB : SV_Target1;
        // rgb - base color
	    // a   - directional light shadow factor
        float4 GBufferC : SV_Target2;
    };

    PixelOutput PS(PixelInput input)
    {
        float3 weight = Sample2D(weight, input.weightTex).xyz;
        float sumWeight = weight.x + weight.y + weight.z;
        weight /= sumWeight;
        float4 diffColor = weight.x * Sample2D(detail1, input.detailTex) +
                            weight.y * Sample2D(detail2, input.detailTex) +
                            weight.z * Sample2D(detail3, input.detailTex);

        float4 matDiffColor = float4(0.5, 0.5, 0.5, 16);
        float4 baseColor = matDiffColor * diffColor;

        PixelOutput output;
        output.GBufferA = float4(input.normal, 1.0);
        output.GBufferB = float4(0.5, 0.5, 0.5, 1.0);
        output.GBufferC = float4(LinearToGammaSpace(baseColor.rgb), 1.0);

        return output;
    }
#endif
