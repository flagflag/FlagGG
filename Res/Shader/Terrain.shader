Shader "Shader Res/Shader/Terrain" {
    Properties {
        _WeightMap("权重图", 2D) = register(0)
        _Detail1Map("细节纹理1", 2D) = register(1)
        _Detail2Map("细节纹理2", 2D) = register(2)
        _Detail3Map("细节纹理3", 2D) = register(3)
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
            }

            Pixel {
                vec3 vNormal : NORMAL = vec3(0.0, 0.0, 1.0)
                vec2 vWeightTex : TEXCOORD0 = vec2(0.0, 0.0)
                vec2 vDetailTex : TEXCOORD1 = vec2(0.0, 0.0)
            }

            CGPROGRAM
            #include "Shader/Engine.h"
            #include "Shader/Define.shader"
            #include "Shader/Common.shader"
            
            #ifdef VERTEX
            void VS()
            {
                mat4 iWorldMatrix = u_model[0];
                vec3 worldPos = mul(iPosition, iWorldMatrix).xyz;
                gl_Position = mul(mul(vec4(worldPos, 1.0), u_view), u_proj);
                vNormal = normalize(mul(vec4(iNormal, 0.0), iWorldMatrix).xyz);          
                vWeightTex = iTexCoord;
                vDetailTex = vec2(32.0, 32.0) * iTexCoord;
            }
            #endif

            #ifdef PIXEL
            void PS()
            {
                vec3 weight = texture2D(_WeightMap, vWeightTex).xyz;
                float sumWeight = weight.x + weight.y + weight.z;
                weight /= sumWeight;
                vec3 diffColor = weight.x * texture2D(_Detail1Map, vDetailTex).rgb +
                                 weight.y * texture2D(_Detail2Map, vDetailTex).rgb +
                                 weight.z * texture2D(_Detail3Map, vDetailTex).rgb;
                gl_FragColor = vec3(diffColor, 1.0);
            }
            #endif
            ENDCG
        }
    }
}
