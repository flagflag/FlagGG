#include "Shader/PBR/BRDF.hlsl"
#ifdef PIXEL

void PBR_BRDF(PBRContext context, out PBRResult result)
{
    float oneMinusReflectivity;
    float3 specularColor;
    context.diffuseColor = DiffuseAndSpecularFromMetallic(context.diffuseColor, context.metallic, context.specular, specularColor, oneMinusReflectivity);
    DisneyBRDF(context, specularColor, oneMinusReflectivity, result);
}

#if defined(DEFERRED_BASEPASS)

// 延迟渲染管线
struct PixelOutput
{
    // rgb - normal
    // a   - ao
    float4 GBufferA : SV_Target0;
    // r - metallic
    // g - specular
    // b - roughness
    // a - 
    float4 GBufferB : SV_Target1;
    // rgb - base color
    // a   - directional light shadow factor
    float4 GBufferC : SV_Target2;
    // rgb - emissive color
    // a   - 
    float4 GBufferD : SV_Target3;
};

PixelOutput PBRPipline(PBRContext context)
{
    PixelOutput output;

    output.GBufferA.rgb = EncodeGBufferNormal(context.normalDirection);
    output.GBufferA.a = context.occlusion;

    output.GBufferB.r = context.metallic;
    output.GBufferB.g = context.specular;
    output.GBufferB.b = context.roughness;
    
    output.GBufferC.rgb = context.diffuseColor;
    output.GBufferC.a = context.shadow;

    output.GBufferD.rgb = context.emissiveColor;

    return output;
}

#else

// 前向渲染管线
struct PixelOutput
{
#if defined(DEFERRED_CLUSTER) && defined(DEBUG_CLUSTER)
    float4 color : SV_target0;
    float4 debugData : SV_target1;
#else
    float4 color : SV_TARGET;
#endif
};

PixelOutput PBRPipline(PBRContext context)
{
    PBRResult result;
    PBR_BRDF(context, result);

    PixelOutput output;
    output.color.rgb = result.finalColor + context.emissiveColor;
    output.color = float4(LinearToGammaSpace(ToAcesFilmic(output.color.rgb)), context.alpha);
#if defined(DEFERRED_CLUSTER) && defined(DEBUG_CLUSTER)
    output.debugData = result.debugData;
#endif
    return output;
}

#endif

#endif
