//
// AABB
//

struct BoundingBox
{
    float4 minPos;
    float4 maxPos;
};

float3 GetBoundingBoxCenter(BoundingBox aabb)
{
    return (aabb.minPos.xyz + aabb.maxPos.xyz) * 0.5f;
}
