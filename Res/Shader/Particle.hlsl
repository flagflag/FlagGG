#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"
#include "Shader/Sampler.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 position        : POSITION;  // xyz: position,  w: relativeTime
        float4 oldPosition     : TEXCOORD0; // xyz: oldPosition, w: particleId
        float4 sizeRotSubImage : TEXCOORD1; // xy: size, z: rotation, w: subImage
        float4 color           : COLOR;     // xyzw: color rgba
    };
#else
    Texture2D colorMap        : register(t0);
    SamplerState colorSampler : register(s0);
    // 材质参数
    cbuffer MaterialParam     : register(b1)
    {
        
    }
#endif

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        float3 worldPos = mul(input.position, worldMatrix);
        float3 oldWorldPos = mul(input.oldPosition, worldMatrix);
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPos;
        output.texcoord = float2(0, 0);
        output.color = input.color;

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        return LinearToGammaSpace(input.color);
    }
#endif
