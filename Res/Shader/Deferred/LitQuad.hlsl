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
    };
#endif

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;     // 屏幕空间uv
    float3 farRay : FAR_RAY;
#ifdef ORTHO
    float3 nearRay : NEAR_RAY;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        float4x3 iWorldMatrix = worldMatrix;
        input.position.w = 1.0;
        float3 worldPos = mul(input.position, iWorldMatrix);
        float4 clipPos  = mul(float4(worldPos, 1.0), projviewMatrix);

        PixelInput output;
        output.position = clipPos;
        output.texcoord = GetQuadTexCoord(clipPos);
        // output.position = float4(input.position.xyz, 1.0);
        // output.texcoord = input.position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        output.farRay = GetFarRay(clipPos);
        #ifdef ORTHO
            output.nearRay = GetNearRay(clipPos);
        #endif

        return output;
    }
#endif

#ifdef PIXEL
    Texture2D GBuffer0Map : register(t0);
    Texture2D GBuffer1Map : register(t1);
    Texture2D GBuffer2Map : register(t2);
    Texture2D GBuffer3Map : register(t3);
    Texture2D depthBuffer : register(t4);
    Texture2D GBufferSSR  : register(t5);

    SamplerState GBuffer0Sampler : register(s0);
    SamplerState GBuffer1Sampler : register(s1);
    SamplerState GBuffer2Sampler : register(s2);
    SamplerState GBuffer3Sampler : register(s3);
    SamplerState depthBufferSampler : register(s4);
    SamplerState GBufferSSRSampler : register(s5);

    PixelOutput PS(PixelInput input)
    {
        float4 GBufferA = GBuffer0Map.SampleLevel(GBuffer0Sampler, input.texcoord, 0);
        float4 GBufferB = GBuffer1Map.SampleLevel(GBuffer1Sampler, input.texcoord, 0);
        float4 GBufferC = GBuffer2Map.SampleLevel(GBuffer2Sampler, input.texcoord, 0);
        float4 GBufferD = GBuffer3Map.SampleLevel(GBuffer3Sampler, input.texcoord, 0);

        PBRContext context;

        context.normalDirection = DecodeGBufferNormal(GBufferA.rgb);
        context.occlusion = GBufferA.a * aoMap.SampleLevel(aoSampler, input.texcoord, 0).r;

        context.metallic = GBufferB.r;
        context.specular = GBufferB.g;
        context.roughness = GBufferB.b;
        
        context.diffuseColor = GBufferC.rgb;
        context.shadow = GBufferC.a;

        context.emissiveColor = GBufferD.rgb;

        // If rendering a directional light quad, optimize out the w divide
    #if REVERSE_Z
        float sceneDepth = ConvertFromDeviceZ(depthBuffer.SampleLevel(depthBufferSampler, input.texcoord, 0).r);
    #else
        float sceneDepth = LinearizeDepth(depthBuffer.SampleLevel(depthBufferSampler, input.texcoord, 0).r, nearClip, farClip);
    #endif
        float sceneDepthNormalize = sceneDepth / farClip;
    #ifdef ORTHO
        float3 worldPosition = lerp(input.nearRay, input.farRay, sceneDepthNormalize);
    #else
        float3 worldPosition = input.farRay * sceneDepthNormalize;
    #endif

        // Position acquired via near/far ray is relative to camera. Bring position to world space
        float3 eyeVec = -worldPosition;
        worldPosition += cameraPos;

    // #ifdef SHADOW
    //     float4 projWorldPos = hvec4_init(worldPosition, 1.0);
    //     float shadow = GetShadowDeferred(projWorldPos, context.normalDirection, sceneDepthNormalize);
    // #else
    //     float shadow = 1.0;
    // #endif

        context.worldPosition = worldPosition;
        context.viewDirection = normalize(eyeVec);
    #if defined(DEFERRED_CLUSTER)
        context.fragCoord = input.texcoord * viewRect.zw;
        context.sceneDepth = sceneDepth;
    #endif

        return PBRPipline(context);
    }
#endif
