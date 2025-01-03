#include "Shader/PBR/PBRCommon.hlsl"
#if defined(CLUSTER)
#include "Shader/PBR/Clusters.hlsl"
#endif
#if defined(NONPUNCTUAL_LIGHTING)
#include "Shader/CapsuleLight.hlsl"
#endif

struct PBRContext
{
    float3 diffuseColor;            // 固有色
    float metallic;                 // 金属度
    float roughness;                // 粗糙度
    float specular;                 // 高光率
    float3 emissiveColor;           // 自发光
    float alpha;                    // 透明度
    float3 worldPosition;           // 世界坐标
    float3 normalDirection;         // 世界坐标法线向量
    float3 viewDirection;           // 物体到相机的向量
    float3 tangentDirecntion;       // 世界坐标切线向量
    float3 bnormalDirection;        // 世界坐标副法线向量
    float shadow;                   // 阴影衰减
    float occlusion;                // 环境光遮蔽
#if defined(CLUSTER)
    float4 fragCoord;
#elif defined(DEFERRED_CLUSTER)
    float2 fragCoord;
    float sceneDepth;
#endif
};

/**
 * @param specularColor - 高光颜色
 * @param roughness     - 粗糙度
 * @param NoV           - normal dot veiwDir
 */
float3 EnvBRDFApprox(float3 specularColor, float roughness, float NdotV)
{
	// [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
	// Adaptation to fit our G term.
	const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
	const float4 c1 = float4(1.0, 0.0425, 1.04, -0.04);
	float4 r = roughness * c0 + c1;
	float a004 = min( r.x * r.x, exp2( -9.28 * NdotV ) ) * r.x + r.y;
	float2 AB = float2( -1.04, 1.04 ) * a004 + r.zw;

	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	// Note: this is needed for the 'specular' show flag to work, since it uses a SpecularColor of 0
	AB.y *= saturate( 50.0 * specularColor.g );

	return specularColor * AB.x + AB.y;
}

float3 GetLightingColor(float3 diffuseColor, float3 specularColor, float roughness, float perceptualRoughness, float3 normalDirection, float3 viewDirection, float3 lightDirection, float NdotV
#ifdef USES_ANISOTROPY
    , float XdotV, float YdotV, float ax, float ay, float3 X, float3 Y
#endif
)
{
// necessary preprocess
    float3 lightVec = normalize(lightDirection);
    float3 halfDirection = normalize(viewDirection + lightDirection);

    float VdotH = clamp(dot(viewDirection, halfDirection), M_EPSILON, 1.0);
    float NdotH = clamp(dot(normalDirection, halfDirection), M_EPSILON, 1.0);
    float NdotL = clamp(dot(normalDirection, lightVec), M_EPSILON, 1.0);
    float LdotH = clamp(dot(lightVec, halfDirection), M_EPSILON, 1.0);

#ifdef USES_ANISOTROPY
    float VdotL = dot(viewDirection, lightVec);
    float invLenH = rsqrt(2.0 + 2.0 * VdotL);

    #if 0
        float XdotL = clamp(dot(X, lightVec), M_EPSILON, 1.0);
        float YdotL = clamp(dot(Y, lightVec), M_EPSILON, 1.0);
    #else
        float XdotL = dot(X, lightVec);
        float YdotL = dot(Y, lightVec);
    #endif

    // float XdotH = clamp(dot(X, halfDirection), M_EPSILON, 1.0);
    // float YdotH = clamp(dot(Y, halfDirection), M_EPSILON, 1.0);
    float XdotH = (XdotL + XdotV) * invLenH;
    float YdotH = (YdotL + YdotV) * invLenH;
#endif

// Diffuse
    float3 directDiffuse = diffuseColor * M_INV_PI;

// VDF高光项
    #ifdef USES_ANISOTROPY
        roughness = max(roughness, 0.002);
        float V = Vis_SmithJointAniso(ax, ay, NdotV, NdotL, XdotV, XdotL, YdotV, YdotL);
        float D = D_GGXaniso(ax, ay, NdotH, XdotH, YdotH);
    #else
        roughness = max(roughness, 0.002);
        float V = Vis_GGX(NdotL, NdotV, roughness);
        float D = D_GGX(NdotH, roughness);
    #endif
    float3 F = FresnelTerm(specularColor, LdotH);
    
// Specular
    float3 directSpecular = V * D * F;

// Diffuse + Specular, NdotL挪到最后乘了
    return (directDiffuse + directSpecular) * NdotL;
}

/**
 * @param context                   - PBR上下文参数
 * @param specularColor             - 高光颜色
 * @param oneMinusReflectivity      - 1.0 - 反色率
 */
float3 DisneyBRDF(PBRContext context, float3 specularColor, float oneMinusReflectivity)
{
    float3 finalColor = float3(0.0, 0.0, 0.0);

// roughness
    float perceptualRoughness = context.roughness;
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    
    float NdotV = abs(dot(context.normalDirection, context.viewDirection));

#ifdef USES_ANISOTROPY
    float ax = 0;
    float ay = 0;
    GetAnisotropicRoughness(perceptualRoughness, anisotropy, ax, ay);

    // X: Tangent
    // Y: Bnormal
    float3 X = context.tangentDirecntion;
    float3 Y = context.bnormalDirection;

    float XdotV = dot(X, context.viewDirection);
    float YdotV = dot(Y, context.viewDirection);
#endif

    float3 lightColor;
    float3 lightDirection;
    float attenuation;
    float3 lightingColor;

// Directional light => light color, light dir, light attenuation
#if defined(DIRLIGHT)
    lightColor = DirectionalLight_GetLightColor();
    attenuation = DirectionalLight_GetAttenAndLightDir(context.worldPosition, lightDirection);

    lightingColor = GetLightingColor(context.diffuseColor, specularColor, roughness, perceptualRoughness, context.normalDirection, context.viewDirection, lightDirection, NdotV
    #ifdef USES_ANISOTROPY
        , XdotV, YdotV, ax, ay, X, Y
    #endif
    );
    finalColor += lightingColor * lightColor * (attenuation * context.shadow);
#endif // DIRLIGHT

// Point/Spot light
#if defined(CLUSTER) || defined(DEFERRED_CLUSTER)
#if defined(DEFERRED_CLUSTER)
    uint cluster = GetDeferredClusterIndex(context.fragCoord.xy, sceneDepth);
#else
    // 这里从gl_FragCoord减去viewport的起点，得到viewport坐标
    float4 realCoord = GetRealCoord(context.fragCoord);
    uint cluster = GetClusterIndex(realCoord);
#endif
    LightGrid grid = GetLightGrid(cluster); // Sample buffer
    for (uint i = 0u; i < grid.pointLights; ++i)
    {
        uint lightIndex = GetGridLightIndex(grid.offset, i); // Sample buffer
#ifdef CLUSTER_SPOTLIGHT
        SpotLight light = GetSpotLight(lightIndex); // Sample buffer
#else
        PointLight light = GetPointLight(lightIndex); // Sample buffer
#endif

        lightColor = light.intensity;
        attenuation = GetAttenAndLightDir(context.worldPosition, light.position, light.range, lightDirection);
#ifdef CLUSTER_SPOTLIGHT
        attenuation *= GetLightDirectionFalloff(lightDirection, light.direction, light.cosOuterCone, light.invCosConeDiff);
#endif

        lightingColor = GetLightingColor(context.diffuseColor, specularColor, roughness, perceptualRoughness, context.normalDirection, context.viewDirection, lightDirection, NdotV
        #ifdef USES_ANISOTROPY
            , XdotV, YdotV, ax, ay, X, Y
        #endif
        );

        finalColor += lightingColor * lightColor * attenuation;
    }

// 非精确光源计算
#ifdef NONPUNCTUAL_LIGHTING
    for (uint i = 0u; i < grid.nonPunctualPointLights; ++i)
    {
        uint lightIndex = GetGridLightIndex(grid.nonPunctualPointLightsOffset, i); // Sample buffer
        NonPunctualPointLight light = GetNonPunctualPointLight(lightIndex); // Sample buffer

        //capsule light
        if (light.length > 0.0)
        {
            finalColor += GetCapsuleLighting(light, context.worldPosition, context.normalDirection, context.viewDirection, context.diffuseColor, specularColor, roughness, perceptualRoughness);
        }
        else if (light.packRadius > 0.0)
        {
            finalColor += GetSphereLighting(light, context.worldPosition, context.normalDirection, context.viewDirection, context.diffuseColor, specularColor, roughness, perceptualRoughness);
        }
    }
#endif
#endif

// Indirect Diffuse, Indirect Specular
#if defined(AMBIENT)
    float3 indirectDiffuse;
    float3 indirectSpecular;
    GI_Indirect(context.normalDirection, context.viewDirection, perceptualRoughness, context.occlusion, indirectDiffuse, indirectSpecular);

    indirectDiffuse *= context.diffuseColor;
    #if 1
        indirectSpecular = indirectSpecular * EnvBRDFApprox(specularColor, perceptualRoughness, NdotV);
    #else
        float surfaceReduction = 1.0 / (roughness * roughness + 1.0);
        float grazingTerm = saturate(1.0 - perceptualRoughness + (1.0 - oneMinusReflectivity));
        indirectSpecular = indirectSpecular * surfaceReduction * FresnelLerp(specularColor, vec3_splat(grazingTerm), NdotV);
    #endif

    finalColor += indirectDiffuse + indirectSpecular;
#endif // AMBIENT

    return finalColor;
}
