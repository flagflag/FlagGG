Shader "Shader Res/Shader/Model" {
    Properties {
        _AmbientColor("Ambient Color", Color) = Color(0, 0, 0)
        _Metallic("Metallic", Range[0, 1]) = 0
        _Gloss("Gloss", Range[0, 1]) = 0.5
    }
    
    SubShader {
        Tags {
            "RenderType" = "LitSolid"
        }

        Pass {
            Name "FORWARD"
            
            Vertex {
                vec4 iPosition : POSITION
                vec3 iNormal : NORMAL
                vec2 iTexCoord : TEXCOORD0
                vec4 iColor : COLOR0
                vec4 iBlendWeight : BLENDWEIGHT
                vec4 iBlendIndices : BLENDINDICES
            }

            Pixel {
                vec3 vNormal : NORMAL = vec3(0.0, 0.0, 1.0)
                vec4 vTangent : TANGENT = vec4(0.0, 0.0, 0.0, 0.0)
                vec2 vTexCoord : TEXCOORD0 = vec2(0.0, 0.0)
                vec4 vColor : COLOR0 = vec4(1.0, 1.0, 1.0, 1.0)
                vec4 vWorldPos : TEXCOORD1 = vec4(0.0, 0.0, 0.0, 0.0)
                vec4 vShadowPos : TEXCOORD2 = vec4(0.0, 0.0, 0.0, 0.0)
            }

            CGPROGRAM
            #include "Shader/Engine.h"
            #include "Shader/Define.shader"
            #include "Shader/Common.shader"
            
            #ifdef VERTEX
            void VS()
            {
            #ifdef STATIC
                mat4 iWorldMatrix = u_model[0];
            #else
                mat4 iWorldMatrix = GetSkinMatrix(iBlendWeight, iBlendIndices);
            #endif

                float3 worldPos = mul(iPosition, iWorldMatrix).xyz;
                // gl_Position = mul(float4(worldPos, 1.0), _ProjViewMatrix);
                gl_Position = mul(mul(vec4(worldPos, 1.0), u_view), u_proj);
                vNormal = normalize(mul(vec4(iNormal, 0.0), iWorldMatrix).xyz);
                vWorldPos = vec4(worldPos, GetDepth(gl_Position));
                vTexCoord = iTexCoord;

                #ifdef COLOR
                    vColor = iColor;
                #endif

            #ifdef SHADOW
                vShadowPos = mul(float4(worldPos, 1.0), _LightProjViewMatrix);
            #endif
            }
            #endif

            #ifdef PIXEL
            float DistributionGGX(vec3 N, vec3 H, float roughness) // roughness 参与
            {
                float PI     = 3.141592654;
                float a      = roughness*roughness;
                float a2     = a*a;
                float NdotH  = max(dot(N, H), 0.0);
                float NdotH2 = NdotH*NdotH;

                float nom   = a2;
                float denom = (NdotH2 * (a2 - 1.0) + 1.0);
                denom = PI * denom * denom;

                return nom / denom;
            }

            float GeometrySchlickGGX(float NdotV, float roughness) // roughness 参与
            {
                float r = (roughness + 1.0);
                float k = (r*r) / 8.0;

                float nom   = NdotV;
                float denom = NdotV * (1.0 - k) + k;

                return nom / denom;
            }

            float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) // roughness 参与
            {
                float NdotV = max(dot(N, V), 0.0);
                float NdotL = max(dot(N, L), 0.0);
                float ggx2  = GeometrySchlickGGX(NdotV, roughness);
                float ggx1  = GeometrySchlickGGX(NdotL, roughness);

                return ggx1 * ggx2;
            }

            vec3 FresnelSchlick(float cosTheta, vec3 F0)
            {
                return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
            }

            void PS()
            {
            //     vec3 albedo = texture2D(_DiffuseMap, vTexCoord);
            //     // r: ao, g: gloss, b: metallic
            //     vec3 aoGlossMetal = texture2D(_SpecularMap, vTexCoord);

            //     float ao = aoGlossMetal.r;
            //     float roughness = 1.0 - aoGlossMetal.g;
            //     float metallic = aoGlossMetal.b;

            //     vec3 N = normalize(vNormal);
            //     vec3 V = normalize(_CameraPos - vWorldPos.xyz);

            //     vec3 F0 = vec3_splat(0.04);
            //     F0 = mix(F0, albedo, metallic); // albedo 参与, metallic 参与, F0 参与

            //     // reflectance equation
            //     vec3 Lo = vec3_splat(0.0);
            //     float PI = 3.141592654;
            //     for (int i = 0; i < 4; ++i)
            //     {
            //         // calculate per-light radiance
            //         vec3 L = normalize(_LightPositions[i].xyz - vWorldPos.xyz);
            //         vec3 H = normalize(V + L);
            //         float distance    = length(_LightPositions[i].xyz - vWorldPos.xyz);
            //         float attenuation = 1.0 / (distance * distance);
            //         vec3 radiance     = _LightColors[i] * attenuation; // light color 参与     

            //         // cook-torrance brdf
            //         float NDF = DistributionGGX(N, H, roughness); // roughness 参与
            //         float G   = GeometrySmith(N, V, L, roughness); // roughness 参与
            //         vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);     

            //         vec3 kS = F;
            //         vec3 kD = vec3_splat(1.0) - kS;
            //         kD *= 1.0 - metallic; // metallic 参与

            //         vec3 nominator    = NDF * G * F;
            //         float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 分母项中加了一个0.001为了避免出现除零错误
            //         vec3 specular     = nominator / denominator;

            //         // add to outgoing radiance Lo
            //         float NdotL = max(dot(N, L), 0.0);
            //         Lo += (kD * albedo / PI + specular) * radiance * NdotL; // albedo 参与
            //     }

            //     vec3 ambient = vec3_splat(0.03) * albedo * ao; // 环境光 term, ao 参与
            //     vec3 color = ambient + Lo;

            //     color = color / (color + vec3_splat(1.0));
            //     color = ToGamma(color);  //转到 gamma 空间

            // // Output
            //     gl_FragColor = vec4(color, 1.0);
                gl_FragColor = vec4(1, 0, 0, 0);
            }
            #endif
            ENDCG
        }
    }
}
