
#ifdef VERTEX
float4 GetShadowPos(float3 worldPos)
{
    float4 shadowClipPos = mul(float4(worldPos, 1.0), lightProjviewMatrix);
    return float4(
		shadowClipPos.x * 0.5 + shadowClipPos.w * 0.5,
		-shadowClipPos.y * 0.5 + shadowClipPos.w * 0.5,
		shadowClipPos.z,
        shadowClipPos.w
	);
}
#else
    #ifdef SHADOW
        Texture2D shadowMap : register(t6);
        SamplerComparisonState shadowSampler : register(s6);

        float GetShadow(float4 inShadowPos)
        {
            float3 shadowPos  = inShadowPos.xyz / inShadowPos.w;
            float2 shadowPos2 = float2(shadowPos.x + shadowMapPixelTexels.x, shadowPos.y);
            float2 shadowPos3 = float2(shadowPos.x, shadowPos.y + shadowMapPixelTexels.y);
            float2 shadowPos4 = float2(shadowPos.xy + shadowMapPixelTexels.xy);
            float4 inLight = float4(
                    shadowMap.SampleCmpLevelZero(shadowSampler,  shadowPos.xy, shadowPos.z).r,
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos2.xy, shadowPos.z).r,
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos3.xy, shadowPos.z).r,
                    shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos4.xy, shadowPos.z).r
                );
            #ifndef SHADOW_CMP
                return saturate(dot(inLight, 0.25));
            #else           
                return saturate(dot(inLight > shadowPos.z, 0.25));
            #endif
        }

        float GetShadow(float4 inShadowPos, float depth)
        {
            float inLight = GetShadow(inShadowPos);
            return saturate(inLight + depth);
        }
    #endif

    TextureCube iblCube : register(t7);
    SamplerState iblSampler : register(s7);

    Texture2D aoMap : register(t8);
    SamplerState aoSampler : register(s8);

    float3 DecodeNormal(float4 normalInput)
    {
    #ifdef PACKEDNORMAL
        float3 normal;
        normal.xy = normalInput.rg * 2.0 - 1.0;
        normal.z = sqrt(max(1.0 - dot(normal.xy, normal.xy), 0.0));
        return normal;
    #else
        return normalize(normalInput.rgb * 2.0 - 1.0);
    #endif
    }

    float ReconstructDepth(float hwDepth)
    {
        return dot(float2(hwDepth, depthReconstruct.y / (hwDepth - depthReconstruct.x)), depthReconstruct.zw);
    }
#endif
