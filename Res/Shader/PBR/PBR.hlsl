#include "Shader/PBR/BRDF.hlsl"
#ifdef PIXEL

float3 PBR_BRDF(PBRContext context)
{
    float oneMinusReflectivity;
    float3 specularColor;
    context.diffuseColor = DiffuseAndSpecularFromMetallic(context.diffuseColor, context.metallic, context.specular, specularColor, oneMinusReflectivity);
    return DisneyBRDF(context, specularColor, oneMinusReflectivity);
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
    float4 color : SV_TARGET;
};

PixelOutput PBRPipline(PBRContext context)
{
    PixelOutput output;
    output.color.rgb = PBR_BRDF(context) + context.emissiveColor;
    output.color = float4(LinearToGammaSpace(ToAcesFilmic(output.color.rgb)), context.alpha);
    return output;
}

#endif

#endif
