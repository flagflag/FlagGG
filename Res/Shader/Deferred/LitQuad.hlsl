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

    SamplerState GBuffer0Sampler : register(s0);
    SamplerState GBuffer1Sampler : register(s1);
    SamplerState GBuffer2Sampler : register(s2);
    SamplerState GBuffer3Sampler : register(s3);
    SamplerState depthBufferSampler : register(s4);

    PixelOutput PS(PixelInput input)
    {
        float4 GBufferA = GBuffer0Map.Sample(GBuffer0Sampler, input.texcoord);
        float4 GBufferB = GBuffer1Map.Sample(GBuffer1Sampler, input.texcoord);
        float4 GBufferC = GBuffer2Map.Sample(GBuffer2Sampler, input.texcoord);
        float4 GBufferD = GBuffer3Map.Sample(GBuffer3Sampler, input.texcoord);

        PBRContext context;

        context.normalDirection = DecodeGBufferNormal(GBufferA.rgb);
        context.occlusion = GBufferA.a;

        context.metallic = GBufferB.r;
        context.specular = GBufferB.g;
        context.roughness = GBufferB.b;
        
        context.diffuseColor = GBufferC.rgb;
        context.shadow = GBufferC.a;

        context.emissiveColor = GBufferD.rgb;

        // If rendering a directional light quad, optimize out the w divide
        float sourceDepth = depthBuffer.Sample(depthBufferSampler, input.texcoord).r;
        float depth = ReconstructDepth(sourceDepth);
        #ifdef ORTHO
            float3 worldPosition = mix(input.nearRay, input.farRay, depth);
        #else
            float3 worldPosition = input.farRay * depth;
        #endif

        // Position acquired via near/far ray is relative to camera. Bring position to world space
        float3 eyeVec = -worldPosition;
        worldPosition += cameraPos;

    // #ifdef SHADOW
    //     float4 projWorldPos = hvec4_init(worldPosition, 1.0);
    //     float shadow = GetShadowDeferred(projWorldPos, context.normalDirection, depth);
    // #else
    //     float shadow = 1.0;
    // #endif

        context.worldPosition = worldPosition;
        context.viewDirection = normalize(eyeVec);
    #if defined(DEFERRED_CLUSTER)
        context.sceneDepth = LinearizeDepth(sourceDepth, nearClip, farClip);
    #endif

        return PBRPipline(context);
    }
#endif
