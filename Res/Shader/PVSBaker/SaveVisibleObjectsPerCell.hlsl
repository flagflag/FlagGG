// Cpu做过Uniform优化，所以uniform的顺序布局不能随便乱改
cbuffer Params : register(b0)
{
    uint modelIDRange;
    uint maxVisibilityIDCountPerCell;
};

RWStructuredBuffer<uint> visibilityMap : register(u0);
// 格子的可见ModelID
struct VisibleIDPerCell
{
    uint startLocation;
    uint validCount;
    uint totalCount;
};
RWStructuredBuffer<VisibleIDPerCell> cellVisibilityIDRange : register(u1);
RWStructuredBuffer<uint> cellVisibilityIDBuffer : register(u2);
// 当前cellVisibilityIDBuffer的count
RWStructuredBuffer<uint> globalIndex : register(u3);
// cellID（原来在cpu端，为了优化cpu设置uniform的开销，把这个挪到gpu累加）
RWStructuredBuffer<uint> currentCellID : register(u4);

#if INIT_VAR
[numthreads(1, 1, 1)]
void CS()
{
    globalIndex[0] = 0;
    currentCellID[0] = 0;
}
#else
[numthreads(1, 1, 1)]
{
    uint index = globalIndex[0];
    uint count = 0;
    for (uint i = 0; i < modelIDRange; ++i)
    {
        if (visibilityMap[i])
        {
            if (count + maxVisibilityIDCountPerCell)
            {
                cellVisibilityIDBuffer[index] = i;
                ++index;
            }
            ++count;
        }
    }

    uint cellID = currentCellID[0];

    VisibleIDPerCell info;
    info.startLocation  = globalIndex[0];
    info.validCount     = index - globalIndex[0];
    info.totalCount     = count;
    cellVisibilityIDRange[cellID] = info;

    globalIndex[0] = index;
    currentCellID[0] = cellID + 1;
}
#endif
