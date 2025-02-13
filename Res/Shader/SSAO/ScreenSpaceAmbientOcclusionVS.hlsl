
void VS(
    in float3 position      : POSITION,
    out float4 outPosition  : SV_POSITION,
    out float2 outTexcoord  : TEXCOORD,
    out float2 outScreenPos : TEXCOORD1)
{
    outPosition = float4(position, 1.0);
    outTexcoord = outPosition.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    outScreenPos = outPosition.xy;
}
