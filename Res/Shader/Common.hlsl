
#ifdef SKINNED
	float4x3 GetSkinMatrix(float4 blendWeights, int4 blendIndices)
	{
		return 
		skinMatrices[blendIndices.x] * blendWeights.x +
		skinMatrices[blendIndices.y] * blendWeights.y +
		skinMatrices[blendIndices.z] * blendWeights.z +
		skinMatrices[blendIndices.w] * blendWeights.w;
	}
#endif

#ifdef VERTEX
float4 GetScreenPos(float4 clipPos)
{
	return float4(
		clipPos.x * 0.5 + clipPos.w * 0.5,
		-clipPos.y * 0.5 + clipPos.w * 0.5,
		0.0,
		clipPos.w
	);
}

float2 GetQuadTexCoord(float4 clipPos)
{
	return float2(
		clipPos.x / clipPos.w * 0.5 + 0.5,
		-clipPos.y / clipPos.w * 0.5 + 0.5
	);
}

float GetDepth(float4 clipPos)
{
    return dot(clipPos.zw, float2(0.0, 1.0 / farClip));
}

float3x3 GetCameraRot()
{
    return (float3x3)invViewMatrix;
}

float3 GetFarRay(float4 clipPos)
{
    float3 viewRay = float3(
        clipPos.x / clipPos.w * frustumSize.x,
        clipPos.y / clipPos.w * frustumSize.y,
        frustumSize.z);

    return mul(viewRay, GetCameraRot());
}

float3 GetNearRay(float4 clipPos)
{
#ifdef ORTHO
    float3 viewRay = float3(
        clipPos.x / clipPos.w * frustumSize.x,
        clipPos.y / clipPos.w * frustumSize.y,
        0.0);
    return mul(viewRay, GetCameraRot());
#else
    return float3(0.0, 0.0, 0.0);
#endif
}

#endif

float LinearizeDepth(float depth, float zNear, float zFar)
{
#if SHADER_LANGUAGE_GLSL
    float z_n = 2.0 * depth - 1.0;
    float linearDepth = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
    return linearDepth;
#else
    float linearDepth = zNear * zFar / (zFar - depth * (zFar - zNear));
    return linearDepth;
#endif
}

float4 EncodeFloatRGBA(float value)
{
	float4 kEncodeMul = float4(1.0, 255.0, 65025.0, 16581375.0);
    float kEncodeBit = 1.0 / 255.0;
    float4 enc = kEncodeMul * value;
    enc = frac(enc);
    enc -= enc.yzww * kEncodeBit;
    return enc;
}

float DecodeFloatRG(float2 enc)
{  
    float2 kDecodeDot = float2(1.0, 1.0 / 255.0);
    return dot(enc, kDecodeDot);
}

float3 EncodeGBufferNormal(float3 normalDirection)
{
    return normalDirection * 0.5 + 0.5;
}

float3 DecodeGBufferNormal(float3 normalDirection)
{
    return normalDirection * 2.0 - 1.0;
}

float GammaToLinearSpaceExact(float value)
{
    if (value <= 0.04045f)
        return value / 12.92f;
    else if (value < 1.0f)
        return pow((value + 0.055f)/1.055f, 2.4f);
    else
        return pow(value, 2.2f);
}

float LinearToGammaSpaceExact(float value)
{
    if (value <= 0.0f)
        return 0.0f;
    else if (value <= 0.0031308f)
        return 12.92f * value;
    else if (value < 1.0f)
        return 1.055f * pow(value, 0.4166667f) - 0.055f;
    else
        return pow(value, 0.45454545f);
}

float3 GammaToLinearSpace(float3 sRGB)
{
    // Approximate version from http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
    return sRGB * (sRGB * (sRGB * 0.305306011f + 0.682171111f) + 0.012522878f);

    // Precise version, useful for debugging.
    //return float3(GammaToLinearSpaceExact(sRGB.r), GammaToLinearSpaceExact(sRGB.g), GammaToLinearSpaceExact(sRGB.b));
}

float4 GammaToLinearSpace(float4 sRGB)
{
    return float4(GammaToLinearSpace(sRGB.rgb), sRGB.a);
}

float3 LinearToGammaSpace(float3 linRGB)
{
    linRGB = max(linRGB, float3(0.f, 0.f, 0.f));
    // An almost-perfect approximation from http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
    return max(1.055f * pow(linRGB, float3(0.416666667f, 0.416666667f, 0.416666667f)) - 0.055f, 0.f);

    // Exact version, useful for debugging.
    //return float3(LinearToGammaSpaceExact(linRGB.r), LinearToGammaSpaceExact(linRGB.g), LinearToGammaSpaceExact(linRGB.b));
}

float4 LinearToGammaSpace(float4 linRGB)
{
    return float4(LinearToGammaSpace(linRGB.rgb), linRGB.a);
}

float3 ToAcesFilmic(float3 rgb)
{
	// Reference(s):
	// - ACES Filmic Tone Mapping Curve
	//   https://web.archive.org/web/20191027010704/https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
	float aa = 2.51f;
	float bb = 0.03f;
	float cc = 2.43f;
	float dd = 0.59f;
	float ee = 0.14f;
	return saturate((rgb * (aa * rgb + bb) ) / (rgb * (cc * rgb + dd) + ee));
}

float Square(float x)
{
    return x * x;
}

float2 Square(float2 x)
{
    return x * x;
}

float3 Square(float3 x)
{
    return x * x;
}

float4 Square(float4 x)
{
    return x * x;
}

float Pow5 (float x)
{
    return x*x * x*x * x;
}
