#ifdef VERTEX
cbuffer HiZParam : register(b0)
{
    float4 screenToUV;
}

void VS(
    in float3 position : POSITION,
    out float4 outPosition : SV_POSITION,
    out float2 outTexcoord : TEXCOORD)
{
    outPosition = float4(position, 1.0);
    outTexcoord = outPosition.xy * screenToUV.xy + screenToUV.zw;
}
#endif

#if PIXEL
Texture2D HiZTexture : register(t0);
Texture2D AABBMinPosTexture : register(t1);
Texture2D AABBMaxPosTexture : register(t2);
SamplerState HiZTextureSampler : register(s0);
SamplerState AABBMinPosTextureSampler : register(s1);
SamplerState AABBMaxPosTextureSampler : register(s2);

cbuffer HiZParam : register(b0)
{
    float4x4 projviewMatrix;
    float2 uvFactor;
    float mipBias;
    float2 HiZTextureSize;
}

void PS(
    in float4 position : SV_POSITION,
    in float2 texcoord : TEXCOORD,
    out float4 outColor : SV_TARGET)
{
    float3 AABBMinPos = AABBMinPosTexture.SampleLevel(AABBMinPosTextureSampler, texcoord, 0).xyz;
    float3 AABBMaxPos = AABBMaxPosTexture.SampleLevel(AABBMaxPosTextureSampler, texcoord, 0).xyz;

	// 将AABB转换到屏幕空间坐标
    float3 AABB[2] = { AABBMinPos, AABBMaxPos };
	float3 rectMin = float3(1, 1, 1);
	float3 rectMax = float3(-1, -1, -1);
    for (int i = 0; i < 8; i++)
    {
        float3 worldPos;
        worldPos.x = AABB[(i >> 0) & 1].x;
        worldPos.y = AABB[(i >> 1) & 1].y;
        worldPos.z = AABB[(i >> 2) & 1].z;

        float4 clipPos = mul(float4(worldPos, 1), projviewMatrix);
        float3 screenPos = clipPos.xyz / clipPos.w;

        rectMin = min(rectMin, screenPos);
        rectMax = max(rectMax, screenPos);
    }

	// FIXME assumes DX
    // 转换到UV空间
	float4 rect = saturate(float4(rectMin.xy, rectMax.xy) * float2(0.5, -0.5).xyxy + 0.5).xwzy;
    // 从0 ~ 1转换到像素个数
	float4 rectPixels = rect * HiZTextureSize.xyxy;
	float2 rectSize = (rectPixels.zw - rectPixels.xy) * 0.5;	// 0.5 for 4x4
	float level = max(ceil(log2(max(rectSize.x, rectSize.y))), mipBias);

	// Check if we can drop one level lower
	float levelLower = max(level - 1, 0);
	float4 lowerRect = rectPixels * exp2(-levelLower);
	float2 lowerRectSize = ceil(lowerRect.zw) - floor(lowerRect.xy);
	if (all(lowerRectSize <= 4))
	{
		level = levelLower;
	}

	// 4x4 samples
	float2 scale = uvFactor.xy * (rect.zw - rect.xy) / 3;
	float2 bias = uvFactor.xy * rect.xy;

	float4 farthestDepth = 0;
    for (int i = 0; i < 4; i++)
    {
        // TODO could vectorize this
        float4 depth;
        depth.x = HiZTexture.SampleLevel(HiZTextureSampler, float2(i, 0) * scale + bias, level).r;
        depth.y = HiZTexture.SampleLevel(HiZTextureSampler, float2(i, 1) * scale + bias, level).r;
        depth.z = HiZTexture.SampleLevel(HiZTextureSampler, float2(i, 2) * scale + bias, level).r;
        depth.w = HiZTexture.SampleLevel(HiZTextureSampler, float2(i, 3) * scale + bias, level).r;
        farthestDepth = max(farthestDepth, depth);
    }
	farthestDepth.x = max(max(farthestDepth.x, farthestDepth.y), max(farthestDepth.z, farthestDepth.w));

	// Inverted Z buffer
	outColor = rectMin.z <= farthestDepth.x ? 1 : 0;
}
#endif
