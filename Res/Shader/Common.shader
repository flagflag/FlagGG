#ifdef SKINNED
	mat4 GetSkinMatrix(vec4 blendWeights, vec4 blendIndices)
	{
		return 
		u_model[int(blendIndices.x)] * blendWeights.x +
		u_model[int(blendIndices.y)] * blendWeights.y +
		u_model[int(blendIndices.z)] * blendWeights.z +
		u_model[int(blendIndices.w)] * blendWeights.w;
	}
#endif

vec4 GetScreenPos(vec4 clipPos)
{
	return vec4(
		clipPos.x * 0.5 + clipPos.w * 0.5,
		-clipPos.y * 0.5 + clipPos.w * 0.5,
		0.0,
		clipPos.w
	);
}

vec2 GetQuadTexCoord(vec4 clipPos)
{
	return vec2(
		clipPos.x / clipPos.w * 0.5 + 0.5,
		-clipPos.y / clipPos.w * 0.5 + 0.5
	);
}

float GetDepth(vec4 clipPos)
{
    return dot(clipPos.zw, vec2(0.0, 1.0 / 1000000000.0));
}

vec4 EncodeFloatRGBA(float value)
{
	vec4 kEncodeMul = vec4(1.0, 255.0, 65025.0, 16581375.0);
    float kEncodeBit = 1.0 / 255.0;
    vec4 enc = kEncodeMul * value;
    enc = fract(enc);
    enc -= enc.yzww * kEncodeBit;
    return enc;
}

float DecodeFloatRG(vec2 enc)
{  
    vec2 kDecodeDot = vec2(1.0, 1.0 / 255.0);
    return dot(enc, kDecodeDot);
}

vec3 LinearColor(vec3 inColor)
{
	return pow(inColor, vec3_splat(2.2));
}

vec4 LinearColor(vec4 inColor)
{
	return vec4(pow(inColor.rgb, vec3_splat(2.2)), 1.0);
}

vec3 ToGamma(vec3 inColor)
{
	return pow(inColor, vec3_splat(1.0 / 2.2));
}

vec4 ToGamma(vec4 inColor)
{
	return vec4(pow(inColor.rgb, vec3_splat(1.0 / 2.2)), 1.0);
}

float GGXTerm(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = NdotH * NdotH * (a2 - 1.f) + 1.f;
	return a2 / (3.141592654 * d * d);
}

float SpecularStrength(vec3 specular)
{
    #if (SHADER_TARGET < 30)
        // SM2.0: instruction count limitation
        // SM2.0: simplified SpecularStrength
        return specular.r; // Red channel - because most metals are either monocrhome or with redish/yellowish tint
    #else
        return max(max(specular.r, specular.g), specular.b);
    #endif
}

vec3 EnergyConservationBetweenDiffuseAndSpecular(vec3 albedo, vec3 specColor, out float oneMinusReflectivity)
{
    oneMinusReflectivity = 1 - SpecularStrength(specColor);
    #if !UNITY_CONSERVE_ENERGY
        return albedo;
    #elif UNITY_CONSERVE_ENERGY_MONOCHROME
        return albedo * oneMinusReflectivity;
    #else
        return albedo * (vec3(1,1,1) - specColor);
    #endif
}

float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
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
    float lambdaV = NdotL * (NdotV * (1 - a) + a);
    float lambdaL = NdotV * (NdotL * (1 - a) + a);

    return 0.5f / (lambdaV + lambdaL + 1e-5f);
#endif
}

float Pow5 (float x)
{
    return x*x * x*x * x;
}

vec3 FresnelTerm(vec3 F0, float cosA)
{
    float t = Pow5(1 - cosA);   // ala Schlick interpoliation
    return F0 + (1 - F0) * t;
}

vec3 FresnelLerp(vec3 F0, vec3 F90, float cosA)
{
    float t = Pow5 (1 - cosA);   // ala Schlick interpoliation
    return mix(F0, F90, t);
}
