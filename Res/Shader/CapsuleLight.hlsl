float SphereHorizonCosWrap(float NoL, float sinAlphaSqr)
{
	float sinAlpha = sqrt(sinAlphaSqr);

	if (NoL < sinAlpha)
	{
		NoL = max(NoL, -sinAlpha);
		NoL = Square(sinAlpha + NoL) / (4.0 * sinAlpha);
	}

	return NoL;
}

float NewRoughness(float roughness, float sinAlpha, float VoH)
{
    return roughness + 0.25 * sinAlpha * (3.0 * sqrt(roughness) + sinAlpha) / (VoH + 0.001);
}

float EnergyNormalization(inout float roughness, float VoH, float sinAlpha, float softSinAlpha, float cosSubtended)
{
	if (softSinAlpha > 0.0)
	{
		// Modify Roughness
		roughness = clamp(roughness + (softSinAlpha * softSinAlpha) / (VoH * 3.6 + 0.4), 0.0, 1.0);
	}

	float sphereRoughness = roughness;
	float energy = 1.0;
	if (sinAlpha > 0.0)
	{
		sphereRoughness = NewRoughness(roughness, sinAlpha, VoH);
		energy = roughness / sphereRoughness;
	}

    if (cosSubtended < 1.0)
    {
        float tanHalfAlpha = sqrt((1.0 - cosSubtended) / (1.0 + cosSubtended));
        float lineRoughness = NewRoughness(sphereRoughness, tanHalfAlpha, VoH);
        energy *= sqrt(sphereRoughness / lineRoughness);
    }

	return energy;
}

// Closest point on line segment to ray
float3 ClosestPointLineToRay(float3 Line0, float3 Line1, float Length, float3 R)
{
	float3 L0 = Line0;
	float3 L1 = Line1;
	float3 Line01 = Line1 - Line0;

	// Shortest distance
	float A = Square(Length);
	float B = dot(R, Line01);
	float t = saturate(dot( Line0, B*R - Line01 ) / (A - B*B));

	return Line0 + t * Line01;
}

// Closest point on sphere to ray
float3 ClosestPointSphereToRay(float3 L, float radius, float3 R)
{
    float3 closestPointOnRay = dot(L, R) * R;
    float3 centerToRay = closestPointOnRay - L;
    float distToRay = sqrt(dot(centerToRay, centerToRay));
    return L + centerToRay * clamp(radius / (distToRay + 1e-4), 0.0, 1.0);
}

float3 GetCapsuleLighting(NonPunctualPointLight light, float3 worldPos, float3 normal, float3 viewDirection, float3 diffuseColor, float3 specularColor, float roughness, float perceptualRoughness)
{
    uint uSoftRadius = uint(light.packRadius * 0.001);
    float radius = light.packRadius - 1000.0 * uSoftRadius;
    float softRadius = (float)uSoftRadius * 0.001;

    float3 L = light.position - worldPos;
    float3 L01 = light.direction * light.length;
    float3 L0 = L - 0.5 * L01;
    float3 L1 = L + 0.5 * L01;
    L01 = L1 - L0;

    float Length0Sqr = dot(L0, L0);
    float Length0 = sqrt(Length0Sqr);
    float Length1Sqr = dot(L1, L1);
    float Length1 = sqrt(Length1Sqr);
    float Length01 = Length0 * Length1;

    float NdotL = 0.5 * (dot(normal, L0) / Length0 + dot(normal, L1) / Length1);
    float distSqr = dot(L, L);
    float rangeMask = GetLightRangeMask(light.range, distSqr);

    float NdotV = abs(dot(normal, viewDirection));
    float cosSubtended = dot(L0, L1) / Length01;
    float falloff = 1.0 / (Length01 * (0.5 * cosSubtended + 0.5 + 1.0 / (Length01 + 1e-4)));
    if (radius > 0.0)
    {
        NdotL = SphereHorizonCosWrap(NdotL, saturate(radius * radius * falloff));
    }

    float3 diff = diffuseColor * M_INV_PI;

    //Specular
    float3 reflectDir = -viewDirection + 2.0 * dot(viewDirection, normal) * normal;
    L = ClosestPointLineToRay(L0, L1, light.length, reflectDir);
    L = ClosestPointSphereToRay(L, radius, reflectDir);
    distSqr = dot(L, L);
    float invDist = 1.0 / sqrt(distSqr + 1e-4);
    falloff *= rangeMask;
    L = normalize(L);
    float3 H = normalize(L + viewDirection);
    float VdotH = dot(viewDirection, H);
    float NdotH = clamp(dot(normal, H), M_EPSILON, 1.0);
    roughness = max(roughness, 0.002);
    float V = Vis_GGX(NdotL, NdotV, roughness);
    float D = D_GGX(NdotH, roughness);
    float3 F = FresnelTerm(specularColor, VdotH);
    float3 spec = V * D * F;
    spec *= EnergyNormalization(roughness, VdotH, radius * invDist * (1.0 - roughness), softRadius * invDist, cosSubtended);

    return (diff + spec) * light.intensity * falloff * NdotL;
}

float3 GetSphereLighting(NonPunctualPointLight light, float3 worldPos, float3 normal, float3 viewDirection, float3 diffuseColor, float3 specularColor, float roughness, float perceptualRoughness)
{
    uint uSoftRadius = uint(light.packRadius * 0.001);
    float radius = light.packRadius - 1000.0 * uSoftRadius;
    float softRadius = (float)uSoftRadius * 0.001;

    float3 lightVec = light.position - worldPos.xyz;
    float distSqr = dot(lightVec, lightVec);
    float rangeMask = GetLightRangeMask(light.range, distSqr);
    float falloff = rangeMask / (distSqr + 1.0);
    float invDist = 1.0 / sqrt(distSqr + 1e-4);
    float sinAlphaSqr = saturate(radius * radius / (distSqr + 1.0));

    float3 lightDirection = lightVec * invDist;
    float NdotL = dot(normal, lightDirection);
    NdotL = SphereHorizonCosWrap(NdotL, sinAlphaSqr);
    float NdotV = abs(dot(normal, viewDirection));

    float3 diff = diffuseColor * M_INV_PI;

    //Specular
    float3 reflectDir = -viewDirection + 2.0 * dot(viewDirection, normal) * normal;
    float3 closestPointOnSphere = ClosestPointSphereToRay(lightVec, radius, reflectDir);

    float3 L = normalize(closestPointOnSphere);
    float3 H = normalize(L + viewDirection);
    float VdotH = dot(viewDirection, H);
    float NdotH = clamp(dot(normal, H), M_EPSILON, 1.0);
    roughness = max(roughness, 0.002);
    float V = Vis_GGX(NdotL, NdotV, roughness);
    float D = D_GGX(NdotH, roughness);
    float3 F = FresnelTerm(specularColor, VdotH);
    float3 spec = V * D * F;
    spec *= EnergyNormalization(roughness, VdotH, radius * invDist * (1.0 - roughness), softRadius * invDist, 1.0);

    return (diff + spec) * light.intensity * falloff * NdotL;
}
