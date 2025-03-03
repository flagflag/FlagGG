// #define KK_LIGHTING 1

// Kajiya-Kay 光照模型
float3 TTShiftTangent(float3 T, float3 N, float shift)
{
    return normalize(T + N * shift);
}

#ifdef KK_LIGHTING
float StrandSpecular(float3 T, float3 V, float3 L, float exponent)
{
    float3 halfDir   = normalize(L + V);
    float dotTH    = dot(T, halfDir);
    float sinTH    = max(0.01, sqrt(1.0 - dotTH * dotTH));
    float dirAtten = smoothstep(-1, 0, dotTH);
    return dirAtten * pow(sinTH, exponent);
}
#else
float YYSpecular(float3 T, float3 V, float3 L, float hairWidth)
{
    float3 halfDir   = L + V;
    float dotTH    = dot(T, halfDir);
    float sinTH    = sqrt(1.0 - dotTH * dotTH);
    return clamp(sinTH - hairWidth, 0.0, 1.0);
}
#endif

struct HairContext
{
    float3 diffuseColor;          // 固有色
    float shift;                  // 发丝shift参数
    float hairWidth;              // 高光宽度
    float hairSpecular;           // 高光系数
    float3 viewDirection;         // 物体到相机的向量
    float3 normalDirection;       // 世界坐标法线向量
    float3 tangentDirection;      // 世界坐标切线向量
    float3 bnormalDirection;      // 世界坐标副法线向量
    float shadow;                 // 阴影衰减
    float occlusion;              // 环境光遮蔽
};

float3 HairSimulationBRDF(HairContext context)
{
    float3 lightVec = lightDir;
    
    #ifdef KK_LIGHTING
        float primaryShift = 0.0;
        float secondaryShift = 0.1;
        float3 specularColor1 = float3(0.1, 0.1, 0.1);
        float3 specularColor2 = float3(0.0, 0.0, 0.0);
        float specExp1 = 0.5;
        float specExp2 = 0.5;

        // shift tangents 
        float _shift = context.shift - 0.5;
        float3 t1 = TTShiftTangent(context.tangentDirection, context.normalDirection, primaryShift + _shift);
        float3 t2 = TTShiftTangent(context.tangentDirection, context.normalDirection, secondaryShift + _shift);
        
        // diffuse lighting: the lerp shifts the shadow boundary for a softer look
        float3 diffuse = saturate(lerp(0.25, 1.0, dot(context.normalDirection, lightVec)));
        diffuse *= context.diffuseColor;

        // specular lighting 
        float3 specular = specularColor1 * StrandSpecular(t1, context.viewDirection, lightVec, specExp1);
        // add 2nd specular term, modulated with noise texture
        float specMask = context.shift; // approximate sparkles using textures
        // 使用纹理近似闪光
        specular += specularColor2 * specMask * StrandSpecular(t2, context.viewDirection, lightVec, specExp2);
    #else
        float3 viewDirection = context.viewDirection;
        #if 1
            float3 row0 = context.tangentDirection;
            float3 row1 = context.bnormalDirection;
            float3 row2 = context.normalDirection;
            float3 normalDirection  = float3(dot(row0, context.normalDirection), dot(row1, context.normalDirection), dot(row2, context.normalDirection));
            float3 tangentDirection = cross(normalDirection, float3(0.0, 1.0, 0.0));
            viewDirection = float3(dot(row0, viewDirection), dot(row1, viewDirection), dot(row2, viewDirection));
        #else
            float3 normalDirection  = context.normalDirection;
            float3 tangentDirection = context.tangentDirection;
        #endif

        // shift tangents
        tangentDirection = TTShiftTangent(tangentDirection, normalDirection, context.shift - 0.5);

        // diffuse lighting: the lerp shifts the shadow boundary for a softer look
        float3 diffuse = saturate(lerp(0.25, 1.0, dot(normalDirection, lightVec)));
        diffuse *= context.diffuseColor * lerp(0.3, 0.45, luma(context.diffuseColor).x);

        // specular lighting 
        float3 specular = YYSpecular(tangentDirection, viewDirection, float3(-0.6324, 0.08569, 0.76929), context.hairWidth) * context.hairSpecular;
        // hairSpecular   |  t
        //      1         | 1.0
        //      2         | 0.25
        //      3         | 0.0625
        //      4         | 0.0625
        //      5         | 0.0625
        float t = max(1.0 / pow(4.0, max(context.hairSpecular - 1.0, 0.0)), 0.0625);
        specular = specular * (smoothstep(0.0, 7.0, luma(lightColor.rgb).x) + t) * 1.5;
    #endif
    
    // final color assembly
    float3 finalColor = min((diffuse * lightColor.rgb + specular) * M_INV_PI, float3(1.0, 1.0, 1.0));
    // finalColor *= context.occlusion;    // modulate color by ambient occlusion term

    return finalColor;
}
