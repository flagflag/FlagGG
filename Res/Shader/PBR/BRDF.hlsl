#include "Shader/PBR/PBRCommon.hlsl"

/**
 * @param specularColor - 高光颜色
 * @param roughness     - 粗糙度
 * @param NoV           - normal dot veiwDir
 */
float3 EnvBRDFApprox(float3 specularColor, float roughness, float NoV)
{
	// [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
	// Adaptation to fit our G term.
	const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
	const float4 c1 = float4(1.0, 0.0425, 1.04, -0.04);
	float4 r = roughness * c0 + c1;
	float a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
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
    float InvLenH = rsqrt( 2.0 + 2.0 * VdotL );

    #if 0
        float XdotL = clamp(dot(X, lightVec), M_EPSILON, 1.0);
        float YdotL = clamp(dot(Y, lightVec), M_EPSILON, 1.0);
    #else
        float XdotL = dot(X, lightVec);
        float YdotL = dot(Y, lightVec);
    #endif

    // float XdotH = clamp(dot(X, halfDirection), M_EPSILON, 1.0);
    // float YdotH = clamp(dot(Y, halfDirection), M_EPSILON, 1.0);
    float XdotH = (XdotL + XdotV) * InvLenH;
    float YdotH = (YdotL + YdotV) * InvLenH;
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
        float V = SmithJointGGXVisibilityTerm(NdotL, NdotV, roughness);
        float D = GGXTerm(NdotH, roughness);
    #endif
    float3 F = FresnelTerm(specularColor, LdotH);
    
// Specular
    float3 directSpecular = V * D * F;

// Diffuse + Specular, NdotL挪到最后乘了
    return (directDiffuse + directSpecular) * NdotL;
}

/**
 * @param diffuseColor              - 已经经过能量守恒的漫反色颜色（和高光做了比例）
 * @param specularColor             - 高光颜色
 * @param oneMinusReflectivity      - 1.0 - 反色率
 * @param roughness                 - 粗糙度比例
 * @param normalDirection           - 法线向量
 * @param viewDirection             - 物体到相机的向量
 * @param shadow                    - 阴影衰减
 * @param occlusion                 - 材质传入的环境光遮蔽
 */
float3 DisneyBRDF(
    float3 diffuseColor,
    float3 specularColor,
    float oneMinusReflectivity,
    float perceptualRoughness,
    float3 worldPosition,
    float3 normalDirection,
    float3 viewDirection,
    float shadow,
    float occlusion)
{
    float3 finalColor = float3(0.0, 0.0, 0.0);

// roughness
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
    
    float NdotV = abs(dot(normalDirection, viewDirection));

    float3 lightColor;
    float3 lightDirection;
    float attenuation;
    float3 lightingColor;

#if defined(DIRLIGHT)
// GI => light color, light dir, light attenuation
    lightColor = GetLightColor();
    attenuation = GetAttenAndLightDir(worldPosition, lightDirection);

    lightingColor = GetLightingColor(diffuseColor, specularColor, roughness, perceptualRoughness, normalDirection, viewDirection, lightDirection, NdotV);
    finalColor += lightingColor * lightColor * (attenuation * shadow);
#endif // DIRLIGHT

#if defined(AMBIENT)
// Indirect Diffuse, Indirect Specular
    float3 indirectDiffuse;
    float3 indirectSpecular;
    GI_Indirect(normalDirection, viewDirection, perceptualRoughness, occlusion, indirectDiffuse, indirectSpecular);

    indirectDiffuse *= diffuseColor;
    indirectSpecular = indirectSpecular * EnvBRDFApprox(specularColor, perceptualRoughness, NdotV);

    finalColor += indirectDiffuse + indirectSpecular;
#endif // AMBIENT

    return finalColor;
}
