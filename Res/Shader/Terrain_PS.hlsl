Texture2D weightMap : register(t0);
Texture2D detail1Map : register(t1);
Texture2D detail2Map : register(t2);
Texture2D detail3Map : register(t3);

SamplerState weightSampler : register(s0);
SamplerState detail1Sampler : register(s1);
SamplerState detail2Sampler : register(s2);
SamplerState detail3Sampler : register(s3);

#define Sample2D(name, tex) name##Map.Sample(name##Sampler, tex)

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 weightTex : TEXCOORD0;
    float2 detailTex : TEXCOORD1;
	float3 nor : NORMAL;
};

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

	return color;
}

