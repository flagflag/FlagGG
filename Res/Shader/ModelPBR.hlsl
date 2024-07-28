#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"
#include "Shader/Lighting.hlsl"
#include "Shader/PBR/PBR.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 pos : POSITION;
        float2 tex : TEXCOORD;
        float3 nor : NORMAL;
    #ifdef COLOR
        float4 color : COLOR;
    #endif
    #ifdef SKINNED
        float4 blendWeights : BLEND_WEIGHTS;
        int4 blendIndices : BLEND_INDICES;
    #endif
    };
#else
    Texture2D colorMap : register(t0);
    SamplerState colorSampler : register(s0);
    #ifdef DISSOLVE
        Texture2D noiseMap : register(t1);
        SamplerState noiseSampler : register(s1);
    #endif
    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float metallic;
        float roughness;
        float4 baseColor;
    }
#endif

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
#ifdef COLOR
	float4 color : COLOR;
#endif
	float4 worldPos : WORLD_POS;
#ifdef SHADOW
	float4 shadowPos : SHADOW_POSITION;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
    #ifdef STATIC
        float4x3 iWorldMatrix = worldMatrix;
    #else
        float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
    #endif
        input.pos.w = 1.0;
        float3 worldPos = mul(input.pos, iWorldMatrix);
        float3 worldNor = normalize(mul(input.nor, (float3x3)iWorldMatrix));
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.pos = clipPos;
        output.tex = input.tex;
        output.nor = worldNor;
    #ifdef COLOR
        output.color = input.color;
    #endif
        output.worldPos = float4(worldPos, GetDepth(clipPos));

    #ifdef SHADOW
        output.shadowPos = GetShadowPos(worldPos);
    #endif

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
    #ifdef SHADOW
        float shadow = GetShadow(input.shadowPos, input.worldPos.w);
    #else
        float shadow = 1.0;
    #endif

    #ifndef COLOR
        float3 diffuseColor = baseColor.rgb * colorMap.Sample(colorSampler, input.tex).rgb;
    #else
        float3 diffuseColor = baseColor.rgb * input.color.rgb * input.color.a;
    #endif
        float3 viewDirection = cameraPos - input.worldPos.xyz;
        float3 color = PBR_BRDF(diffuseColor, metallic, roughness, input.worldPos.xyz, input.nor, viewDirection, shadow, 1.0);

        return float4(LinearToGammaSpace(ToAcesFilmic(color)), 1.0);
    }
#endif
