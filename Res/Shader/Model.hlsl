#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 pos : POSITION;
        float2 tex : TEXCOORD;
        float3 nor : NORMAL;
    #ifdef COLOR
        float4 color : COLOR;
    #endif
    #ifdef SKINNED
        float4 blendWeights : BLEND_WEIGHTS;
        int4 blendIndices : BLEND_INDICES;
    #endif
    };
#else
    Texture2D colorMap : register(t0);
    SamplerState colorSampler : register(s0);
    #ifdef DISSOLVE
        Texture2D noiseMap : register(t1);
        SamplerState noiseSampler : register(s1);
    #endif
    #ifdef SHADOW
        Texture2D shadowMap : register(t6);
        SamplerState shadowSampler : register(s6);
    #endif

    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
        float4 ambientColor;
        float4 diffuseColor;
        float4 specularColor;
        float emissivePower;
        float dissolveTime;
    }
#endif

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
#ifdef COLOR
	float4 color : COLOR;
#endif
	float3 worldPos : WORLD_POS;
#ifdef SHADOW
	float4 shadowPos : POSITION;
#endif
};

#ifdef VERTEX
    PixelInput VS(VertexInput input)
    {
    #ifdef STATIC
        float4x3 iWorldMatrix = worldMatrix;
    #else
        float4x3 iWorldMatrix = GetSkinMatrix(input.blendWeights, input.blendIndices);
    #endif
        input.pos.w = 1.0;
        float3 worldPos = mul(input.pos, iWorldMatrix);
        float3 worldNor = normalize(mul(input.nor, (float3x3)iWorldMatrix));
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.pos = clipPos;
        output.tex = input.tex;
        output.nor = worldNor;
    #ifdef COLOR
        output.color = input.color;
    #endif
        output.worldPos = worldPos;

    #ifdef SHADOW
        float4 shadowClipPos = mul(float4(worldPos, 1.0), lightProjviewMatrix);
        output.shadowPos = shadowClipPos;
    #endif

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float lightIntensity = saturate(dot(input.nor, -lightDir));
        float3 diffColor = saturate(diffuseColor.rgb * lightIntensity);
    #ifndef COLOR
        float3 textureColor = colorMap.Sample(colorSampler, input.tex).rgb;
    #else
        float3 textureColor = input.color.rgb * input.color.a;
    #endif
        diffColor = diffColor * textureColor;

        float3 reflectDir = (input.nor * -lightDir * 2.0) * input.nor - (-lightDir);
        float3 eyeDir = cameraPos - input.worldPos;
        float3 specColor = specularColor.rgb * pow(saturate(dot(eyeDir, reflectDir)), emissivePower);

    #ifdef SHADOW
        float bias = 0.001;
        float2 shadowTex;
        shadowTex.x = input.shadowPos.x / input.shadowPos.w * 0.5 + 0.5;
        shadowTex.y = input.shadowPos.y / input.shadowPos.w * (-0.5) + 0.5;
        if (saturate(shadowTex.x) == shadowTex.x && saturate(shadowTex.y) == shadowTex.y)
        {
            float shadowDepth = DecodeFloatRG(shadowMap.Sample(shadowSampler, shadowTex).xy) + bias;
            float depth = input.shadowPos.z / input.shadowPos.w;
            if (shadowDepth < depth)
            {
                // diffColor = float3(0, 0, 0);
            }
        }
    #endif

        float ambiPower = ambientColor.a / 255.0;
        float diffPower = diffuseColor.a / 255.0;
        float specPower = specularColor.a / 255.0;
        float4 color = float4(ambientColor.rgb * ambiPower + diffColor * diffPower + specColor * specPower, 1.0);

    // 溶解特效
    #ifdef DISSOLVE
        float dissolveNoise = noiseMap.Sample(noiseSampler, input.tex).r;
        float temp = dissolveNoise + dissolveTime;
        if (temp > 1)
            discard;
        if (temp > 0.9)
            color += float4(0.94, 0.9, 0.54, 0);
    #endif

        return color;
    }
#endif
