#ifdef PIXEL
#define SAMPLE_FULL_SH_PER_PIXEL 1

// normal should be normalized, w=1.0
float3 SHEvalLinearL0L1 (float4 normal)
{
    float3 x;

    // Linear (L1) + constant (L0) polynomial terms
    x.r = dot(SHAr, normal);
    x.g = dot(SHAg, normal);
    x.b = dot(SHAb, normal);

    return x;
}

// normal should be normalized, w=1.0
float3 SHEvalLinearL2(float4 normal)
{
    float3 x1, x2;
    // 4 of the quadratic (L2) polynomials
    float4 vB = normal.xyzz * normal.yzzx;
    x1.r = dot(SHBr, vB);
    x1.g = dot(SHBg, vB);
    x1.b = dot(SHBb, vB);

    // Final (5th) quadratic (L2) polynomial
    float vC = normal.x * normal.x - normal.y * normal.y;
    x2 = SHC.rgb * vC;

    return x1 + x2;
}

float3 ShadeSHPerPixel(float3 normal, float3 ambient, float shIntensity)
{
    float3 ambientContrib = float3(0.0, 0.0, 0.0);

    // 只有high及以上渲染质量才开全sh
    #if SAMPLE_FULL_SH_PER_PIXEL
        // Completely per-pixel
        ambientContrib = SHEvalLinearL0L1(float4(normal, 1.0));
        ambientContrib += SHEvalLinearL2(float4(normal, 1.0));
        // 混合球谐（移动端平片地表使用顶点球谐，而悬崖使用像素球谐，边界处存在硬边，所以使用混合球谐让它过度柔和）
        #if VERTEX_PIXEL_SH_BLEND
            ambientContrib = mix(ambientContrib, vAmbientColor.rgb, vAmbientColor.a);
        #endif
        ambient += max(float3(0.0, 0.0, 0.0), ambientContrib * shIntensity);
    #else
        // L2 per-vertex, L0..L1 & gamma-correction per-pixel
        // Ambient in this case is expected to be always Linear, see ShadeSHPerVertex()
        ambientContrib = SHEvalLinearL0L1(float4(normal, 1.0));
        // 混合球谐（移动端平片地表使用顶点球谐，而悬崖使用像素球谐，边界处存在硬边，所以使用混合球谐让它过度柔和）
        #if VERTEX_PIXEL_SH_BLEND
            ambientContrib = mix(ambientContrib, vAmbientColor.rgb, vAmbientColor.a);
        #endif
        ambient = max(float3(0.0, 0.0, 0.0), ambient + ambientContrib * shIntensity);     // include L2 contribution in vertex shader before clamp.
    #endif

    return ambient;
}

void GI_Indirect(float3 normalDirection, float3 viewDirection, float perceptualRoughness, float occlusion,
    out float3 indirectDiffuse, out float3 indirectSpecular)
{
    perceptualRoughness = perceptualRoughness * (1.7 - 0.7*perceptualRoughness);
    float mip = perceptualRoughness * 6.0; //  perceptualRoughness * SPECCUBE_LOD_STEPS

    float NdotV = clamp(dot(normalDirection, viewDirection), 0.0, 1.0);
    float3 viewReflection = 2.0 * NdotV * normalDirection - viewDirection; // Same as: -reflect(viewDirection, normalDirection);

    float3 cubeN = normalDirection;
    cubeN.xy = float2(dot(cubeN.xy, float2(envCubeAngle.y, -envCubeAngle.x)), dot(cubeN.xy, envCubeAngle.xy));
    cubeN = float3(cubeN.x, cubeN.z, cubeN.y);
    indirectDiffuse = ShadeSHPerPixel(cubeN, float3(0.0, 0.0, 0.0), shIntensity);

    float3 cubeR = viewReflection;
    cubeR.xy = float2(dot(cubeR.xy, float2(envCubeAngle.w, -envCubeAngle.z)), dot(cubeR.xy, envCubeAngle.zw));
    cubeR = float3(cubeR.x, cubeR.z, cubeR.y);
    float4 filterGGX = iblCube.SampleLevel(iblSampler, cubeR, mip);
    #ifndef CUBE_RGBM
        indirectSpecular = filterGGX.xyz * iblIntensity; // radiance
    #else
        indirectSpecular = filterGGX.xyz * filterGGX.w * 6.0 * iblIntensity; // radiance
    #endif

    indirectDiffuse = indirectDiffuse * occlusion * ambientOcclusionIntensity;
    indirectSpecular = indirectSpecular * occlusion * ambientOcclusionIntensity;
}
#endif
