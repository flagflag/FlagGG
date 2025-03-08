#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"
#include "Shader/Lighting.hlsl"
#include "Shader/GI/Ambient.hlsl"
#include "Shader/PBR/PBR.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float3 position : POSITION;
    #ifndef NO_TEXTURE
        float2 texcoord : TEXCOORD;
    #endif
        float3 normal : NORMAL;
    #ifdef VERTEX_COLOR
        float4 color : COLOR;
    #endif
    #ifdef SKINNED
        float4 blendWeights : BLEND_WEIGHTS;
        int4 blendIndices : BLEND_INDICES;
    #endif
    #ifdef INSTANCE
        float4 instanceData0 : INSTANCE0;
        float4 instanceData1 : INSTANCE1;
        float4 instanceData2 : INSTANCE2;
    #endif
    };
#else
    #ifndef NO_TEXTURE
        Texture2D colorMap : register(t0);
        SamplerState colorSampler : register(s0);
    #endif
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
#ifndef NO_TEXTURE
	float2 texcoord : TEXCOORD;
#endif
	float3 normal : NORMAL;
#ifdef VERTEX_COLOR
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
        #ifdef INSTANCE
            float4x3 iWorldMatrix = float4x3(
                float3(input.instanceData0.x, input.instanceData1.x, input.instanceData2.x),
                float3(input.instanceData0.y, input.instanceData1.y, input.instanceData2.y),
                float3(input.instanceData0.z, input.instanceData1.z, input.instanceData2.z),
                float3(input.instanceData0.w, input.instanceData1.w, input.instanceData2.w)
                );
        #else
            float4x3 iWorldMatrix = worldMatrix;
        #endif
    #else
        float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
    #endif
        float3 worldPosition = mul(float4(input.position, 1.0), iWorldMatrix);
        float3 worldNormal = normalize(mul(input.normal, (float3x3)iWorldMatrix));
        float4 clipPos = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPos;
    #ifndef NO_TEXTURE
        output.texcoord = input.texcoord;
    #endif
        output.normal = worldNormal;
    #ifdef VERTEX_COLOR
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

    #ifdef NO_TEXTURE
        context.diffuseColor = float(1.0).xxx;
    #else
        context.diffuseColor = baseColor.rgb * colorMap.Sample(colorSampler, input.texcoord).rgb;
    #endif

    #ifdef VERTEX_COLOR
        context.diffuseColor = context.diffuseColor * input.color.rgb * input.color.a;
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
