
#ifdef VERTEX
float4 GetShadowPos(float3 worldPos)
{
    float4 shadowClipPos = mul(float4(worldPos, 1.0), lightProjviewMatrix);
    return float4(
		shadowClipPos.x * 0.5 + shadowClipPos.w * 0.5,
		-shadowClipPos.y * 0.5 + shadowClipPos.w * 0.5,
		0.0,
        shadowClipPos.w
	);
}
#else
    #ifdef SHADOW
        Texture2D shadowMap : register(t6);
        #ifdef SHADOW_CMP
            SamplerComparisonState shadowSampler : register(s6);
        #else
            SamplerState shadowSampler : register(s6);
        #endif

        float GetShadow(float4 inShadowPos)
        {
            float3 shadowPos  = inShadowPos.xyz / inShadowPos.w;
            float3 shadowPos2 = float3(shadowPos.x + shadowMapPixelTexels.x, shadowPos.yz);
            float3 shadowPos3 = float3(shadowPos.x, shadowPos.y + shadowMapPixelTexels.y, shadowPos.z);
            float3 shadowPos4 = float3(shadowPos.xy + shadowMapPixelTexels.xy, shadowPos.z);
            #ifdef SHADOW_CMP
                float4 inLight = float4(
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos.xy, shadowPos.z).r,
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos2.xy, shadowPos2.z).r,
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos3.xy, shadowPos3.z).r,
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos4.xy, shadowPos4.z).r
                );
                return saturate(dot(inLight, 0.25)  + 0.25);
            #else           
                float4 inLight = float4(
                    shadowMap.Sample(shadowSampler, shadowPos.xy).r,
                    shadowMap.Sample(shadowSampler, shadowPos2.xy).r,
                    shadowMap.Sample(shadowSampler, shadowPos3.xy).r,
                    shadowMap.Sample(shadowSampler, shadowPos4.xy).r
                );
                return saturate(dot(inLight > shadowPos.z, 0.25) + 0.25);
            #endif
        }
    #endif
#endif
