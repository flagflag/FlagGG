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
        float3 normal : NORMAL;
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
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
#ifdef COLOR
	float4 color : COLOR;
#endif
	float4 worldPosition : WORLD_POS;
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
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, iWorldMatrix);
        float3 worldNormal = normalize(mul(input.normal, (float3x3)iWorldMatrix));
        float4 clipPos = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPos;
        output.texcoord = input.texcoord;
        output.normal = worldNormal;
    #ifdef COLOR
        output.color = input.color;
    #endif
        output.worldPosition = float4(worldPosition, GetDepth(clipPos));

    #ifdef SHADOW
        output.shadowPos = GetShadowPos(worldPosition);
    #endif

        return output;
    }
#else
    PixelOutput PS(PixelInput input)
    {
        PBRContext context;

    #ifdef SHADOW
        context.shadow = GetShadow(input.shadowPos, input.worldPosition.w);
    #else
        context.shadow = 1.0;
    #endif

    #ifndef COLOR
        context.diffuseColor = baseColor.rgb * colorMap.Sample(colorSampler, input.texcoord).rgb;
    #else
        context.diffuseColor = baseColor.rgb * input.color.rgb * input.color.a;
    #endif
        context.metallic = metallic;
        context.roughness = roughness;
        context.specular = 0.5;
        context.emissiveColor = float3(0.0, 0.0, 0.0);
        context.worldPosition = input.worldPosition.xyz;
        context.normalDirection = normalize(input.normal);
        context.viewDirection = normalize(cameraPos - input.worldPosition.xyz);;
        context.occlusion = 1.0;
        context.alpha = 1.0;

        return PBRPipline(context);
    }
#endif
