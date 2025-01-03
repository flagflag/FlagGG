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
    PixelOutput PS(PixelInput input)
    {
        PBRContext context;

    #ifdef SHADOW
        context.shadow = GetShadow(input.shadowPos, input.worldPos.w);
    #else
        context.shadow = 1.0;
    #endif

    #ifndef COLOR
        context.diffuseColor = baseColor.rgb * colorMap.Sample(colorSampler, input.tex).rgb;
    #else
        context.diffuseColor = baseColor.rgb * input.color.rgb * input.color.a;
    #endif
        context.viewDirection = cameraPos - input.worldPos.xyz;
        context.metallic = metallic;
        context.roughness = roughness;
        context.specular = 0.5;
        context.emissiveColor = float3(0.0, 0.0, 0.0);
        context.worldPosition = input.worldPos.xyz;
        context.normalDirection = normalize(input.nor);
        context.viewDirection = viewDirection;
        context.occlusion = 1.0;
        context.alpha = 1.0;

        return PBRPipline(context);
    }
#endif
