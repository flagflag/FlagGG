#include "Shader/Define.hlsl"
#include "Shader/Common.hlsl"

#ifdef VERTEX
    struct VertexInput
    {
        float4 pos : POSITION;
        float2 tex : TEXCOORD;
        float3 nor : NORMAL;
    };
    cbuffer MaterialParam : register(b3)
    {
        float noiseTiling;
        float2 noiseSpeed;
    }
#else
    Texture2D noiseMap : register(t0);
    SamplerState noiseSampler : register(s0);

    Texture2D diffMap : register(t1);
    SamplerState diffSampler : register(s1);

    Texture2D colorMap : register(t2);
    SamplerState colorSampler : register(s2);

    Texture2D envMap : register(t5);
    SamplerState envSampler : register(s5);

    // 材质参数
    cbuffer MaterialParam : register(b1)
    {
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
#ifdef RAIN_DOT
    float2 uv : TEXCOORD2;
#endif
};

#ifdef VERTEX
    inline float3 GerstnerWavePosition(float3 pos, float Q, float A, float3 D, float w, float f, float t)
    {
        float v = dot(D.xz, pos.xz);
        return float3(
            Q * A * D.x * cos(w * v + f * t),
            A * sin(w * v + f * t),
            Q * A * D.z * cos(w * v + f * t)
        );
    }

    inline float3 GerstnerWaveNormal(float3 pos, float Q, float A, float3 D, float w, float f, float t)
    {
        float v = dot(D.xz, pos.xz);
        return -1.0 * float3(
            D.x * w * A * cos(w * v + f * t),
            Q * w * A * sin(w * v + f * t),
            D.z * w * A * cos(w * v + f * t)
        );
    }
    
    PixelInput VS(VertexInput input)
    {
#ifdef PEACEFUL_WAVE
        float time = elapsedTime * 2.0;
        float power = 0.1;
        float2 dir = normalize(float2(1.0, 1.0));
        float a = 1.0 * power;
        float w = 0.3;
        float q = 1.0;
        float t = 0;
        input.pos.y = 0;
        input.pos.y += a * sin(dot(dir, input.pos.xz) * w + time * q);
        t += a * w * cos(dot(dir, input.pos.xz) * w + time * q);

        dir = normalize(float2(1.0, 0.0));
        a = 0.4 * power;
        w = 0.1;
        q = 1.0;
        input.pos.y += a * sin(dot(dir, input.pos.xz) * w + time * q);
        t += a * w * cos(dot(dir, input.pos.xz) * w + time * q);

        dir = normalize(float2(0.0, 2.0));
        a = 0.6 * power;
        w = 0.2;
        q = 0.5;
        input.pos.y += a * sin(dot(dir, input.pos.xz) * w + time * q);
        t += a * w * cos(dot(dir, input.pos.xz) * w + time * q);

        dir = normalize(float2(1.0, 2.0));
        a = 0.7 * power;
        w = 0.15;
        q = 0.1;
        input.pos.y += a * sin(dot(dir, input.pos.xz) * w + time * q);
        t += a * w * cos(dot(dir, input.pos.xz) * w + time * q);

        input.nor = float3(-t, 1.0, -t);
#elif TURBULENT_WAVE
        float time = elapsedTime * 2.0;
        float3 pos = float3(input.pos.x, 0, input.pos.z);
        float3 nor = float3(0, 1, 0);

        float A1 = 1.5;
        float3 D1 = normalize(float3(1.0, 0.0, 1.0));
        float w1 = 0.3;
        float f1 = 1.0;
        float Q1 = 1.0 / (A1 * w1) / 4;

        float A2 = 0.6;
        float3 D2 = normalize(float3(1.0, 0.0, 0.0));
        float w2 = 0.1;
        float f2 = 0.7;
        float Q2 = 1.0 / (A2 * w2) / 4;

        float A3 = 2.1;
        float3 D3 = normalize(float3(0.0, 0.0, 2.0));
        float w3 = 0.2;
        float f3 = 0.5;
        float Q3 = 1.0 / (A3 * w3) / 4;

        float A4 = 2.2;
        float3 D4 = normalize(float3(1.0, 0.0, 2.0));
        float w4 = 0.15;
        float f4 = 0.2;
        float Q4 = 1.0 / (A4 * w4) / 4;

        float A5 = 1.8;
        float3 D5 = normalize(float3(-1.0, 0.0, 1.0));
        float w5 = 0.3;
        float f5 = 0.8;
        float Q5 = 1.0 / (A5 * w5) / 4;

        pos += GerstnerWavePosition(input.pos.xyz, Q1, A1, D1, w1, f1, time);
        pos += GerstnerWavePosition(input.pos.xyz, Q2, A2, D2, w2, f2, time);
        pos += GerstnerWavePosition(input.pos.xyz, Q3, A3, D3, w3, f3, time);
        pos += GerstnerWavePosition(input.pos.xyz, Q4, A4, D4, w4, f4, time);
        pos += GerstnerWavePosition(input.pos.xyz, Q5, A5, D5, w5, f5, time);

        nor += GerstnerWaveNormal(pos, Q1, A1, D1, w1, f1, time);
        nor += GerstnerWaveNormal(pos, Q2, A2, D2, w2, f2, time);
        nor += GerstnerWaveNormal(pos, Q3, A3, D3, w3, f3, time);
        nor += GerstnerWaveNormal(pos, Q4, A4, D4, w4, f4, time);
        nor += GerstnerWaveNormal(pos, Q5, A5, D5, w5, f5, time);

        input.pos.xyz = pos;
        input.nor = normalize(nor);
#endif

        input.pos.w = 1.0;
        float3 worldPos = mul(input.pos, worldMatrix);
        float3 worldNor = normalize(mul(input.nor, (float3x3)worldMatrix));
        float4 clipPos = mul(float4(worldPos, 1.0), projviewMatrix);
        
        PixelInput output;
        output.pos = clipPos;
        output.tex = input.tex * noiseTiling * noiseSpeed * elapsedTime;
        output.nor = worldNor;
        output.screenPos = GetScreenPos(clipPos);
        output.reflectTex = GetQuadTexCoord(clipPos) * clipPos.w;
        output.eyeVec = float4(cameraPos - worldPos, GetDepth(clipPos));
#ifdef RAIN_DOT
        output.uv = input.tex;
#endif

        return output;
    }
#else
    float4 PS(PixelInput input) : SV_TARGET
    {
        float4 color = float4(1, 1, 1, 1);

        float lightIntensity = saturate(dot(input.nor, -lightDir));
        float3 diffColor = saturate(float3(0.88, 0.88, 0.88) * lightIntensity) * waterColor;

        float3 reflectDir = (input.nor * -lightDir * 2.0) * input.nor - (-lightDir);
        float3 eyeDir = input.eyeVec.xyz / input.eyeVec.w;
        float3 specColor = float3(0.02, 0.02, 0.02) * pow(saturate(dot(eyeDir, reflectDir)), 1);

        float3 ambiColor = float3(0.1, 0.1, 0.1) * waterColor;

        color.xyz = diffColor + specColor + ambiColor;
        
        return color;
    }
#endif
