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
	float3 worldPosition : WORLD_POS;
#ifdef SHADOW
	float4 shadowPos : POSITION;
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
        output.worldPosition = worldPosition;

    #ifdef SHADOW
        output.shadowPos = GetShadowPos(worldPosition);
    #endif

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
    #ifdef SHADOW
        float shadow = GetShadow(input.shadowPos);
    #else
        float shadow = 1.0;
    #endif

    #ifdef PBR_NOTEXTURE
        float4 texDiff = float4(1.0, 1.0, 1.0, 1.0);
    #else
        float4 texDiff = colorMap.Sample(colorSampler, input.texcoord);
    #endif

    #ifdef ALPHAMASK
        if (texDiff.a < 0.5)
            discard;
    #endif

    #ifdef ALPHA_DIFF_A
        float4 baseColor = texDiff * colorFactor;
    #else
        float4 baseColor = float4(texDiff.rgb, 1.0) * colorFactor;
    #endif

    #ifdef NORMALMAP
        float4 texNormal = mixMap.Sample(mixSampler, input.texcoord);
        float3 normal = DecodeNormal(texNormal);
        float metallic = texNormal.a * metallicFactor;
        float roughness = texNormal.b * roughnessFactor;

        float3 row0 = float3(input.tangent.x, input.biNormal.x, input.normal.x);
        float3 row1 = float3(input.tangent.y, input.biNormal.y, input.normal.y);
        float3 row2 = float3(input.tangent.z, input.biNormal.z, input.normal.z);
        float3 normalDirection = float3(dot(row0, normal), dot(row1, normal), dot(row2, normal));
    #else
        float metallic = metallicFactor;
        float roughness = roughnessFactor;
        float3 normalDirection = input.normal;
    #endif
        metallic *= metalMul0or1;

        float3 viewDirection = normalize(cameraPos - input.worldPosition);
        float3 color = PBR_BRDF(baseColor, metallic, roughness, input.worldPosition, normalDirection, viewDirection, shadow, 1.0);

    #ifdef EMISSIVECOLOR
        #ifdef ALPHAMASK
            color = color + baseColor.rgb * 2.5 + saturate(texDiff.a - 0.6);
        #else
            color = color + baseColor.rgb * texDiff.a;
        #endif
    #endif

        return float4(LinearToGammaSpace(color), 1.0);
    }
#endif