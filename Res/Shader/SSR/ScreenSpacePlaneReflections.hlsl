//
// 屏幕空间平面反射
//

#include "Shader/Platform.hlsl"
#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"
#include "Shader/Random.hlsl"

#ifdef VERTEX
void VS(
    in float3 position : POSITION,
    out float4 outPosition : SV_POSITION,
    out float2 outTexcoord : TEXCOORD,
    out float3 outFarRay : FAR_RAY)
{
    float4x3 iWorldMatrix = worldMatrix;
    float3 worldPos = mul(float4(position, 1.0), iWorldMatrix);
    float4 clipPos  = mul(float4(worldPos, 1.0), projviewMatrix);

    outPosition = float4(position, 1.0);
    outTexcoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
}
#endif

#if PIXEL
// rgb: 屏幕空间法线
Texture2D GBuffer0Map : register(t0);
SamplerState GBuffer0Sampler: register(s0);

// b: 粗糙度
Texture2D GBuffer1Map : register(t1);
SamplerState GBuffer1Sampler : register(s1);

// rgb: base color
Texture2D GBuffer2Map : register(t2);
SamplerState GBuffer2Sampler : register(s2);

// 屏幕空间深度
Texture2D depthBuffer : register(t4);
SamplerState depthBufferSampler : register(s4);

cbuffer SSPRParam : register(b1)
{
    float4 reflectionPlane;
    float4 planarReflectionOrigin;
    float4 planarReflectionXAxis;
    float4 planarReflectionYAxis;
    float2 planarReflectionScreenBound;
    float4x3 inverseTransposeMirrorMatrix;
    bool bIsStereo;
    float3 planarReflectionParameters;
    float2 planarReflectionParameters2;
    float4x4 projectionWithExtraFOV[2];
    float4 planarReflectionScreenScaleBias[2];
    uint stereoPassIndex;
}

half4 ComputePlanarReflections(float3 worldPosition, half3 normalDirection, float3 viewDirection, half roughness)
{
	half4 outPlanarReflection = 0;

	float planeDistance = dot(reflectionPlane, float4(worldPosition, -1));
	half distanceFade = 1 - saturate(abs(planeDistance) * planarReflectionParameters.x + planarReflectionParameters.y);

	float3 planeOriginToWorldPosition = worldPosition - planarReflectionOrigin.xyz;
	float xAxisDistance = dot(planeOriginToWorldPosition, planarReflectionXAxis.xyz);
	half xAxisFade = saturate((planarReflectionXAxis.w - abs(xAxisDistance)) * planarReflectionParameters.x);
	float yAxisDistance = dot(planeOriginToWorldPosition, planarReflectionYAxis.xyz);
	half yAxisFade = saturate((planarReflectionYAxis.w - abs(yAxisDistance)) * planarReflectionParameters.x);
	distanceFade *= xAxisFade * yAxisFade;

	half angleFade = saturate(dot(reflectionPlane.xyz, normalDirection) * planarReflectionParameters2.x + planarReflectionParameters2.y);
	half roughnessFade = 1 - saturate((roughness - .2f) * 10.0f);
	half finalFade = distanceFade * angleFade * roughnessFade;

	BRANCH
	if (finalFade > 0)
	{
		// Reflect the effective ReflectionVector in mirrored space to get the original camera vector
		float3 mirroredCameraVector = reflect(viewDirection, -reflectionPlane.xyz);
		// Transform the GBuffer normal into mirrored space
		half3 mirroredNormal = mul(normalDirection, inverseTransposeMirrorMatrix).xyz;
		// Reflect the original camera vector across the GBuffer normal in mirrored space
		half3 mirroredReflectionVectorOffNormal = reflect(mirroredCameraVector, mirroredNormal);
		// At this point we have a new reflection vector off of the GBuffer normal, and we need to approximate its intersection with the scene
		// An accurate intersection would ray trace the planar reflection depth buffer
		// As an approximation we are just intersecting with a user defined sphere
		float3 virtualReflectionSpherePosition = worldPosition + mirroredReflectionVectorOffNormal * planarReflectionParameters.z;
		// Transform the intersection position into view space
		float3 viewVirtualReflectionSpherePosition = mul(float4(virtualReflectionSpherePosition, 1), viewMatrix).xyz;
		// Transform the intersection position into clip space using the same projection matrix used to render PlanarReflectionTexture
		float4 clipVirtualReflectionSpherePosition = mul(float4(viewVirtualReflectionSpherePosition, 1), projectionWithExtraFOV[stereoPassIndex]);

		uint eyeIndex = 0;
		if (bIsStereo)
		{
			eyeIndex = stereoPassIndex;
		}

		half2 NDC = clamp(clipVirtualReflectionSpherePosition.xy / clipVirtualReflectionSpherePosition.w, -planarReflectionScreenBound, planarReflectionScreenBound);
		half2 viewportUV = NDC * planarReflectionScreenScaleBias[eyeIndex].xy + planarReflectionScreenScaleBias[eyeIndex].zw;

		half4 planarReflectionTextureValue = GBuffer2Map.SampleLevel(GBuffer2Sampler, viewportUV, 0);

		// Fade out in regions of the planar reflection that weren't written to, so we can composite with other reflection methods
		finalFade *= planarReflectionTextureValue.a;
		outPlanarReflection.rgb = planarReflectionTextureValue.rgb * roughnessFade; // Add roughness fade to color to provide smooth color transition.
		outPlanarReflection.a = finalFade;
	}

	return outPlanarReflection;
}

void PS(
    in float4 position : SV_POSITION,
    in float2 texcoord : TEXCOORD,
    in float3 farRay : FAR_RAY,
    out float4 outColor : SV_TARGET)
{
    float normalDirection = DecodeGBufferNormal(GBuffer0Map.SampleLevel(GBuffer0Sampler, texcoord, 0).rgb);
    float roughness = GBuffer1Map.SampleLevel(GBuffer1Sampler, texcoord, 0).b;
    float sceneDepth = ConvertFromDeviceZ(depthBuffer.SampleLevel(depthBufferSampler, texcoord, 0).r);
    // 世界空间位置
    float3 worldPosition = farRay * sceneDepth;
    float3 viewDirection = normalize(-worldPosition);
    worldPosition += cameraPos;
    // 计算平面反射
    outColor = ComputePlanarReflections(worldPosition, normalDirection, viewDirection, roughness);
}

#endif
