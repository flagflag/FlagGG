
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
    return dot(clipPos.zw, float2(0.0, 1.0 / 1000000000.0));
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
