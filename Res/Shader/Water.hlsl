#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 pos : POSITION;
        float2 tex : TEXCOORD;
        float3 nor : NORMAL;
    };
#else
    Texture2D noiseMap : register(t0);
    SamplerState noiseSampler : register(s0);

    Texture2D diffMap : register(t1);
    SamplerState diffSampler : register(s1);

    Texture2D envMap : register(t5);
    SamplerState envSampler : register(s5);

    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float noiseTiling;
        float2 noiseSpeed;
        float noiseStrength;
        float3 waterColor;
        float fresnelPower;
    }
#endif

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
	float4 screenPos : SCREENPOS;
	float2 reflectTex : TEXCOORD1;
	float4 eyeVec : EYEVEC;
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
        input.pos.w = 1.0;
        float3 worldPos = mul(input.pos, worldMatrix);
        float3 worldNor = normalize(mul(input.nor, (float3x3)worldMatrix));
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.pos = clipPos;
        output.tex = input.tex;
        output.nor = worldNor;
        output.screenPos = GetScreenPos(clipPos);
        output.reflectTex = GetQuadTexCoord(clipPos) * clipPos.w;
        output.eyeVec = float4(cameraPos - worldPos, GetDepth(clipPos));

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float4 color = float4(1, 1, 1, 1);

        float2 refractTex = input.screenPos.xy / input.screenPos.w;
        float2 reflectTex = input.reflectTex.xy / input.screenPos.w;

        float2 tex = input.tex * noiseTiling * noiseSpeed * elapsedTime;
        float2 noise = (noiseMap.Sample(noiseSampler, tex).rg - 0.5) * noiseStrength;
        
        refractTex += noise;
        if (noise.y < 0.0)
            noise.y = 0.0;
        reflectTex += noise;

        float3 refractColor = envMap.Sample(envSampler, refractTex).rgb * waterColor;
        float3 reflectColor = diffMap.Sample(diffSampler, reflectTex).rgb;

        float fresnel = pow(1.0 - saturate(dot(normalize(input.eyeVec.xyz), input.nor)), fresnelPower);

        color = float4(lerp(refractColor, reflectColor, fresnel), 1.0);
        
        return color;
    }
#endif
