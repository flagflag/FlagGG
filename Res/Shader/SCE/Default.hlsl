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
    Texture2D mixMap : register(t1);
    SamplerState mixSampler : register(s1);
    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float4 colorFactor;
        float metallicFactor;
        float4 roughnessFactor;
        float metalMul0or1;
        float emissiveMul;
        float4 emissiveColor;
    }
#endif

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float3 biNormal : BINORMAL;
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
        float3 worldNormal   = normalize(mul(input.normal, (float3x3)iWorldMatrix));
        float3 worldTangent  = normalize(mul(input.tangent.xyz, (float3x3)iWorldMatrix));
        float3 worldBiNormal = cross(worldTangent, worldNormal) * input.tangent.w;
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPosition;
        output.texcoord = input.texcoord;
        output.normal   = worldNormal;
        output.tangent  = worldTangent;
        output.biNormal = worldBiNormal;
    #ifdef COLOR
        output.color    = input.color;
    #endif
        output.worldPosition = float4(worldPosition, GetDepth(clipPosition));

    #ifdef SHADOW
        output.shadowPos = GetShadowPos(worldPosition);
    #endif

        return output;
    }
#else
    PixelOutput PS(PixelInput input)
    {
    #ifdef PBR_NOTEXTURE
        float4 texDiff = float4(1.0, 1.0, 1.0, 1.0);
    #else
        float4 texDiff = colorMap.Sample(colorSampler, input.texcoord);
    #endif

    #ifdef ALPHAMASK
        if (texDiff.a < 0.5)
            discard;
    #endif

        PBRContext context;
        context.diffuseColor = texDiff.rgb * colorFactor;

    #ifdef NORMALMAP
        float4 texNormal = mixMap.Sample(mixSampler, input.texcoord);
        float3 normal = DecodeNormal(texNormal);
        context.metallic = texNormal.a * metallicFactor;
        context.roughness = texNormal.b * roughnessFactor;

        float3 row0 = float3(input.tangent.x, input.biNormal.x, input.normal.x);
        float3 row1 = float3(input.tangent.y, input.biNormal.y, input.normal.y);
        float3 row2 = float3(input.tangent.z, input.biNormal.z, input.normal.z);
        context.normalDirection = normalize(float3(dot(row0, normal), dot(row1, normal), dot(row2, normal)));
    #else
        context.metallic = metallicFactor;
        context.roughness = roughnessFactor;
        context.normalDirection = normalize(input.normal);
    #endif
        context.metallic *= metalMul0or1;
        context.specular = 0.5;

        context.worldPosition = input.worldPosition.xyz;
        context.viewDirection = normalize(cameraPos - input.worldPosition.xyz);
        context.tangentDirection = input.tangent;
        context.bnormalDirection = input.biNormal;
    #ifdef SHADOW
        context.shadow = GetShadow(input.shadowPos, input.worldPosition.w);
    #else
        context.shadow = 1.0;
    #endif
        context.occlusion = 1.0;

    #ifdef EMISSIVECOLOR
        #ifdef ALPHAMASK
            context.emissiveColor = context.diffuseColor * 2.5 + saturate(texDiff.a - 0.6) * emissiveMul * emissiveColor.rgb;
        #else
            context.emissiveColor = context.diffuseColor * texDiff.a * emissiveMul * emissiveColor.rgb;
        #endif
    #else
        context.emissiveColor = float3(0.0, 0.0, 0.0);
    #endif

    #ifdef ALPHA_DIFF_A
        context.alpha = texDiff.a;
    #else
        context.alpha = 1.0;
    #endif

        return PBRPipline(context);
    }
#endif
