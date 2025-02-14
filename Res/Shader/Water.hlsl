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
    };
    cbuffer MaterialParam : register(b3)
    {
        float2 waterSpeed;
        float2 waveScaler;
        float waveDensity;
    }
#else
    Texture2D refractionMap : register(t0);
    SamplerState refractionSampler : register(s0);

    Texture2D depthBuffer : register(t1);
    SamplerState depthBufferSampler : register(s1);

    Texture2D waterNormalMap : register(t2);
    SamplerState waterNormalSampler : register(s2);

    TextureCube reflectionCube : register(t3);
    SamplerState reflectionSampler : register(s3);

    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float noiseStrength;
        float normalStrength;
        float4 waterColor0;
        float4 waterColor1;
        float specularGloss;
        float specularFactor;
        float fresnelFactor;
        float reflection;
        float waterReflectionIntensity;
    }
#endif

struct PixelInput
{
	float4 position   : SV_POSITION;
	float2 uv1        : TEXCOORD1;
    float2 uv2        : TEXCOORD2;
	float4 screenPos  : SCREENPOS;
    float3 worldPosition : WORLDPOS;
#ifdef RAIN_DOT
    float2 uv : TEXCOORD3;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.position.w = 1.0;
        float3 worldPosition = mul(input.position, worldMatrix);
        float4 clipPosition  = mul(float4(worldPosition, 1.0), projviewMatrix);

        PixelInput output;
        output.position = clipPosition;
        output.uv1 = waveDensity * (worldPosition.yx / waveScaler.x + waterSpeed * elapsedTime);
        output.uv2 = waveDensity * (worldPosition.yx / waveScaler.y + waterSpeed * elapsedTime);
        output.screenPos = GetScreenPos(clipPosition);
        output.worldPosition = worldPosition;
#ifdef RAIN_DOT
        output.uv = input.texcoord;
#endif

        return output;
    }
#else
    struct PixelOutputT
    {
    #if DEBUG_WATER_DEPTH
        float4 color    : SV_Target0;
        float4 debugRet : SV_Target1;
    #else
        float4 color    : SV_TARGET;
    #endif
    };

    PixelOutputT PS(PixelInput input) : SV_TARGET
    {
        float3 viewDirection = normalize(cameraPos - input.worldPosition);
        float3 texNormal = DecodeNormal(waterNormalMap.Sample(waterNormalSampler, input.uv1)) + DecodeNormal(waterNormalMap.Sample(waterNormalSampler, input.uv2));
        float3 worldNormal = normalize(texNormal);

        float2 refractUV = input.screenPos.xy / input.screenPos.w;
        float2 noise = worldNormal.xy * noiseStrength; // worldNormal.xy作为distortion系数
        
        float sceneDepth = ConvertFromDeviceZ(depthBuffer.Sample(depthBufferSampler, refractUV + noise).r);
        if (sceneDepth > input.screenPos.w)
            refractUV += noise;
        float3 refrCol = GammaToLinearSpace(refractionMap.Sample(refractionSampler, refractUV).rgb) * waterColor1;

        // 高光
        float3 worldNormalNoise = lerp(float3(0.0, 0.0, 1.0), worldNormal, normalStrength);
        float NdotV = clamp(dot(worldNormalNoise, viewDirection), 0.0, 1.0);
        float3 viewReflection = 2.0 * NdotV * worldNormalNoise - viewDirection; // Same as: -reflect(viewDirection, worldNormalNoise);
        float3 cubeR = viewReflection.xzy;
        float3 reflCol = reflectionCube.SampleLevel(reflectionSampler, cubeR, 0.0).rgb;
        reflCol = GammaToLinearSpace(reflCol);
        // 与环境高光关联
        reflCol = reflCol * waterReflectionIntensity * M_PI;

        float fresnel = pow(1.0 - NdotV, fresnelFactor);
        float reflIntensity = clamp(reflection * fresnel, 0.0, 1.0);

        float3 color = lerp(refrCol, reflCol, reflIntensity);

        PixelOutputT output;
        output.color = float4(LinearToGammaSpace(ToAcesFilmic(color)), 1.0);
#if DEBUG_WATER_DEPTH
        output.debugRet = float4(sceneDepth, input.position.w, input.screenPos.w, 1);
#endif

	    return output;
    }
#endif
