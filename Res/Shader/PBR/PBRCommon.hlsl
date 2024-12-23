#define FlagGG_ColorSpaceDielectricSpec float4(0.08, 0.08, 0.08, 1.0)

#ifndef FLAGGG_CONSERVE_ENERGY
    #define FLAGGG_CONSERVE_ENERGY 1
#endif
#ifndef FLAGGG_CONSERVE_ENERGY_MONOCHROME
    #define FLAGGG_CONSERVE_ENERGY_MONOCHROME 1
#endif

float PerceptualRoughnessToRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

float PerceptualRoughnessToSpecPower(float perceptualRoughness)
{
    float m = PerceptualRoughnessToRoughness(perceptualRoughness);   // m is the true academic roughness.
    float sq = max(1e-4f, m*m);
    float n = (2.0 / sq) - 2.0;                          // https://dl.dropboxusercontent.com/u/55891920/papers/mm_brdf.pdf
    n = max(n, 1e-4f);                                   // prevent possible cases of pow(0,0), which could happen when roughness is 1.0 and NdotH is zero
    return n;
}

float SpecularStrength(float3 specular)
{
    return max(max(specular.r, specular.g), specular.b);
}

float3 EnergyConservationBetweenDiffuseAndSpecular(float3 albedo, float3 specColor, out float oneMinusReflectivity)
{
    oneMinusReflectivity = 1.0 - SpecularStrength(specColor);
    #if !FLAGGG_CONSERVE_ENERGY
        return albedo;
    #elif FLAGGG_CONSERVE_ENERGY_MONOCHROME
        return albedo * oneMinusReflectivity;
    #else
        return albedo * (float3(1.0,1.0,1.0) - specColor);
    #endif
}

float OneMinusReflectivityFromMetallic(float metallic)
{
    // We’ll need oneMinusReflectivity, so
    //   1-reflectivity = 1-lerp(dielectricSpec, 1, metallic) = lerp(1-dielectricSpec, 0, metallic)
    // store (1-dielectricSpec) in FlagGG_ColorSpaceDielectricSpec.a, then
    //   1-reflectivity = lerp(alpha, 0, metallic) = alpha + metallic*(0 - alpha) =
    //                  = alpha - metallic * alpha
    float oneMinusDielectricSpec = FlagGG_ColorSpaceDielectricSpec.a;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

float3 DiffuseAndSpecularFromMetallic(float3 albedo, float metallic, float specular, out float3 specColor, out float oneMinusReflectivity)
{
    specColor = lerp(FlagGG_ColorSpaceDielectricSpec.rgb * specular, albedo, metallic);
    oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
    return albedo * oneMinusReflectivity;
}

// Convert a roughness and an anisotropy factor into GGX alpha values respectively for the major and minor axis of the tangent frame
void GetAnisotropicRoughness(float alpha, float anisotropy, out float ax, out float ay)
{
#if 1
	// Anisotropic parameters: ax and ay are the roughness along the tangent and bitangent	
	// Kulla 2017, "Revisiting Physically Based Shading at Imageworks"
	ax = max(alpha * (1.0 + anisotropy), 0.001);
	ay = max(alpha * (1.0 - anisotropy), 0.001);
#else
	float k = sqrt(1.0f - 0.95f * anisotropy);
	ax = max(alpha / k, 0.001f);
	ay = max(alpha * k, 0.001f);
#endif
}

// Trowbridge-Reitz GGX
float D_GGX(float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = (NdotH * a2 - NdotH) * NdotH + 1.0;  // 2 mad
    return M_INV_PI * a2 / (d * d + 1e-4);     // This function is not intended to be running on Mobile,
                                                    // therefore epsilon is smaller than what can be represented by half
}

// Anisotropic GGX
// [Burley 2012, "Physically-Based Shading at Disney"]
float D_GGXaniso( float ax, float ay, float NoH, float XoH, float YoH )
{
// The two formulations are mathematically equivalent
#if 1
	float a2 = ax * ay;
	float3 v = float3(ay * XoH, ax * YoH, a2 * NoH);
	float s = dot(v, v);

	return M_INV_PI * a2 * Square(a2 / s);
#else
	float d = XoH * XoH / (ax * ax) + YoH * YoH / (ay * ay) + NoH * NoH;
	return 1.0f / ( M_PI * ax*ay * d*d );
#endif
}

// Visibility - GGX - Smith Joint
float Vis_GGX(float NdotL, float NdotV, float roughness)
{
#if 0
    // Original formulation:
    //  lambda_v    = (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
    //  lambda_l    = (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
    //  G           = 1 / (1 + lambda_v + lambda_l);

    // Reorder code to be more optimal
    float a          = roughness;
    float a2         = a * a;

    float lambdaV    = NdotL * sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
    float lambdaL    = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

    // Simplify visibility term: (2.0f * NdotL * NdotV) /  ((4.0f * NdotL * NdotV) * (lambda_v + lambda_l + 1e-5f));
    return 0.5f / (lambdaV + lambdaL + 1e-5f);  // This function is not intended to be running on Mobile,
                                                // therefore epsilon is smaller than can be represented by half
#else
    // Approximation of the above formulation (simplify the sqrt, not mathematically correct but close enough)
    float a = roughness;
    float lambdaV = NdotL * (NdotV * (1.0 - a) + a);
    float lambdaL = NdotV * (NdotL * (1.0 - a) + a);

    return 0.5 / (lambdaV + lambdaL + 1e-4);
#endif
}

// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointAniso(float ax, float ay, float NoV, float NoL, float XoV, float XoL, float YoV, float YoL)
{
	float vis_SmithV = NoL * length(float3(ax * XoV, ay * YoV, NoV));
	float vis_SmithL = NoV * length(float3(ax * XoL, ay * YoL, NoL));
	return 0.5 / (vis_SmithV + vis_SmithL);
}

float3 FresnelTerm(float3 F0, float cosA)
{
    float t = Pow5(1.0 - cosA);   // ala Schlick interpoliation
    return F0 + (1.0 - F0) * t;
}

float3 FresnelLerp(float3 F0, float3 F90, float cosA)
{
    float t = Pow5 (1.0 - cosA);   // ala Schlick interpoliation
    return lerp(F0, F90, t);
}
