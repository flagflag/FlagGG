#include "Shader/PBR/BRDF.hlsl"

/**
 * @param diffuseColor              - 固有色
 * @param metallic                  - 金属度
 * @param roughness                 - 粗糙度
 * @param worldPosition             - 世界坐标
 * @param normalDirection           - 法线向量
 * @param viewDirection             - 物体到相机的向量
 * @param shadow                    - 阴影衰减
 * @param occlusion                 - 环境光遮蔽
 */
float3 PBR_BRDF(
    float3 diffuseColor,
    float metallic,
    float roughness,
    float3 worldPosition,
    float3 normalDirection,
    float3 viewDirection,
    float shadow,
    float occlusion)
{
    float oneMinusReflectivity;
    float3 specularColor;
    diffuseColor = DiffuseAndSpecularFromMetallic(diffuseColor, metallic, specularColor, oneMinusReflectivity);
    return DisneyBRDF(diffuseColor, specularColor, oneMinusReflectivity, roughness, worldPosition, normalDirection, viewDirection, shadow, occlusion);
}
