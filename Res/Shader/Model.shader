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

                vec3 worldPos = mul(iPosition, iWorldMatrix).xyz;
                gl_Position = mul(mul(vec4(worldPos, 1.0), u_view), u_proj);
                vNormal = normalize(mul(vec4(iNormal, 0.0), iWorldMatrix).xyz);
                vWorldPos = vec4(worldPos, GetDepth(gl_Position));
                vTexCoord = iTexCoord;

                #ifdef COLOR
                    vColor = iColor;
                #endif

            #ifdef SHADOW
                vShadowPos = mul(vec4(worldPos, 1.0), _LightProjViewMatrix);
            #endif
            }
            #endif

            #ifdef PIXEL
            void PS()
            {
                vec3 diffColor = texture2D(_DiffuseMap, vTexCoord).rgb;
                gl_FragColor = vec4(diffColor, 0.0);
            }
            #endif
            ENDCG
        }
    }
}
