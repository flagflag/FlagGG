// Cpu做过Uniform优化，所以uniform的顺序布局不能随便乱改
cbuffer Params : register(b0)
{
    uint modelIDRange;
    uint2 textureSize;
    uint sampleCount;
};

Texture2DArray<uint> modelIDTexture : register(t0);
RWStructuredBuffer<uint> visibilityMap : register(u1);

#if INIT_VAR
[numthreads(THREAD_COUNT, 1, 1)]
void CS(uint32 DTid : SV_DispatchThreadID)
{
    if (DTid.x < modelIDRange)
    {
        visibilityMap[DTid.x] = 0;
    }
}
#else
[numthreads(THREAD_X_COUNT, THREAD_Y_COUNT, THREAD_Z_COUNT)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    // 超过纹理size
    if (DTid.x >= textureSize.x ||
        DTid.y >= textureSize.y)
        return;

    for (uint i = 0; i < sampleCount; ++i)
    {
        // 先这样，后面改成SampleGrad加速采样缓存
        uint modelID = modelIDTexture.Load(uint4(DTid.xy, i), 0);
        // 校验下modelID是否合法，避免Gpu挂了（Gpu挂了超级难查）
        if (modelID < modelIDRange)
        {
            // 并发写很慢，差了5倍效率
            // uint temp = 0;
            // InterlockedAdd(visibilityMap[modelID], 1, temp);
            visibilityMap[modelID] = 1;
        }
    }
}
#endif
