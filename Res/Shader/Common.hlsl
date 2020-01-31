
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
