#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
    #ifdef VERTEX_POSITION
        float4 position     : POSITION;
    #endif
    #ifdef VERTEX_NORMAL
        float3 normal       : NORMAL;
    #endif
    #ifdef VERTEX_TEXCOORD
        float2 texcoord     : TEXCOORD;
    #endif
    #ifdef VERTEX_TEXCOORD2
        float2 texcoord2    : TEXCOORD;
    #endif
    #ifdef VERTEX_COLOR
        float4 color        : COLOR;
    #endif
    #ifdef SKINNED
        float4 blendWeights : BLEND_WEIGHTS;
        int4   blendIndices : BLEND_INDICES;
    #endif
    };
#endif


struct PixelInput
{
	float4 position : SV_POSITION;
#ifdef VERTEX_NORMAL
    float3 normal   : NORMAL;
#endif
#ifdef VERTEX_TEXCOORD
    float2 texcoord : TEXCOORD;
#endif
#ifdef VERTEX_COLOR
    float4 color    : COLOR;
#endif
    float3 worldPos : WORLDPOSITION;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
    #ifdef STATIC
        float4x3 iWorldMatrix = worldMatrix;
    #else
        float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
    #endif

        input.position.w = 1.0;
        float3 worldPos = mul(input.position, iWorldMatrix);
        float3 worldNor = normalize(mul(input.nor, (float3x3)iWorldMatrix));
        float4 clipPos  = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.position = clipPos;
        output.texcoord = input.tex;
        output.normal   = worldNor;
    #ifdef COLOR
        output.color = input.color;
    #endif
        output.worldPos = worldPos;
    }
#endif

#ifdef PIXEL
    Texture2D colorMap : register(t0);
    SamplerState colorSampler : register(s0);

    float4 PS(PixelInput input) : SV_TARGET
    {
    #ifdef VERTEX_COLOR
        float3 baseColor = colorMap.Sample(colorSampler, input.texcoord).rgb * input.color.rgb;
    #else
        float3 baseColor = colorMap.Sample(colorSampler, input.texcoord).rgb;
    #endif
        
    }
#endif
