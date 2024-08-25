#ifdef VERTEX
    struct VertexInput
    {
        float4 position : POSITION;
    };
#endif

struct PixelInput
{
	float4 position : SV_POSITION;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        PixelInput output;
        output.position = input.position;
        return output;
    }
#else
    cbuffer MaterialParam : register(b0)
    {
        float floatValue;
        float floatArrayValue[2];
        float2 float2Value;
        float2 float2ArrayValue[2];
        float3 float3Value;
        float3 float3ArrayValue[2];
        float4 float4Value;
        float4 float4ArrayValue[2];
        float3x3 mat3x3Value;
        float4x4 mat4x4Value;
    };

    float4 PS(PixelInput input) : SV_TARGET
    {
        float4 color;
        color.x = floatValue + floatArrayValue[0] + floatArrayValue[1];
        color.y = float2Value.x + float2Value.y + float2ArrayValue[0].x + float2ArrayValue[0].y + float2ArrayValue[1].x + float2ArrayValue[1].y;
        color.z = float3Value.x + float3Value.y + float3Value.z + float3ArrayValue[0].x + float3ArrayValue[0].y + float3ArrayValue[0].z + float3ArrayValue[1].x + float3ArrayValue[1].y + float3ArrayValue[1].z;
        color.w = float4Value.x + float4Value.y + float4Value.z + float4Value.w + 
                    float4ArrayValue[0].x + float4ArrayValue[0].y + float4ArrayValue[0].z + float4ArrayValue[0].w +
                    float4ArrayValue[1].x + float4ArrayValue[1].y + float4ArrayValue[1].z + float4ArrayValue[1].w;
        
	    return color;
    }
#endif
