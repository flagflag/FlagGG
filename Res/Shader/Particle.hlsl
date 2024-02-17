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
        float2 texcoord        : TEXCOORD2;
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
        float3 worldPosition    = mul(input.position, worldMatrix);
        // worldPosition.xy        += input.sizeRotSubImage.xy * input.texcoord;
        worldPosition.xy        += float2(1, 1) * input.texcoord;
        float3 oldWorldPosition = mul(input.oldPosition, worldMatrix);
        float4 clipPosition     = mul(float4(worldPosition, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPosition;
        output.texcoord = float2(0, 0);
        // output.color = input.color;
        output.color = float4(1, 0, 0, 1);

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        return LinearToGammaSpace(input.color);
    }
#endif
