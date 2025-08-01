#if VERTEX
    cbuffer PVSDrawIndirectUniform : register(b0)
    {
        float4x4 projViewMatrix;
    };

    void VS(
        in float3 position      : POSITION,
        in float4 instanceData0 : INSTANCE0,
        in float4 instanceData1 : INSTANCE1,
        in float4 instanceData2 : INSTANCE2,
        in uint   instanceData3 : INSTANCE3,
    #if DEBUG_VIEW
        in float4 instanceData4 : INSTANCE4,
    #endif
        out float4 outPosition  : SV_POSITION,
        out uint   outModelID   : MODEL_ID
    #if DEBUG_VIEW
      , out float4 outDebugData : DEBUG_DATA
    #endif
    )
    {
        float4x3 worldMatrix = float4x3(
            float3(instanceData0.x, instanceData1.x, instanceData2.x),
            float3(instanceData0.y, instanceData1.y, instanceData2.y),
            float3(instanceData0.z, instanceData1.z, instanceData2.z),
            float3(instanceData0.w, instanceData1.w, instanceData2.w)
        );
        float3 worldPosition = mul(float4(position, 1.0), worldMatrix);
        float3 clipPosition = mul(float4(worldPosition, 1.0), projViewMatrix);
        outPosition = clipPosition;
        outModelID = instanceData3;
    #if DEBUG_VIEW
        outDebugData = instanceData4;
    #endif
    }
#elif PIXEL
    void PS(
        in float4   position        : SV_POSITION,
        in uint     modelID         : MODEL_ID,
    #if DEBUG_VIEW
        in float4   debugData       : DEBUG_DATA,
        out uint4   outColor        : SV_Target0,
        out float4  outColor2       : SV_Target1
    #else
        out uint4   outColor        : SV_TARGET
    #endif
    )
    {
        outColor.r = modelID;
    #if DEBUG_VIEW
        outColor2 = debugData;
    #endif
    }
#endif
