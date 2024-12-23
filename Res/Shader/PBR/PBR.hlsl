#include "Shader/PBR/BRDF.hlsl"

float3 PBR_BRDF(PBRContext context)
{
    float oneMinusReflectivity;
    float3 specularColor;
    context.diffuseColor = DiffuseAndSpecularFromMetallic(context.diffuseColor, context.metallic, context.specular, specularColor, oneMinusReflectivity);
    return DisneyBRDF(context, specularColor, oneMinusReflectivity);
}
