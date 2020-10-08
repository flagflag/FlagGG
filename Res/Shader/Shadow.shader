Shader "Shader Res/Shader/Shadow" {
    Properties {
        
    }

    SubShader {
        Tags {
            "RenderType" = "Shadow"
        }

        Pass {
            Name "SHADOW"
            
            Vertex {
                vec4 iPosition : POSITION
                vec4 iNormal : NORMAL
                vec2 iTexCoord : TEXCOORD0
                vec4 iColor : COLOR0
                vec4 iBlendWeight : BLENDWEIGHT
                vec4 iBlendIndices : BLENDINDICES
            }

            Pixel {}

            CGPROGRAM
            #include "Shader/Define.shader"
            #include "Shader/Common.shader"

            void VS()
            {
            #ifdef STATIC
                mat4 iWorldMatrix = _WorldMatrix;
            #else
                mat4 iWorldMatrix = GetSkinMatrix(iBlendWeight, iBlendIndices);
            #endif

                float3 worldPos = mul(iPosition, iWorldMatrix).xyz;
                gl_Position = mul(float4(worldPos, 1.0), _ProjViewMatrix);
            }

            void PS()
            {
                gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            }
            ENDCG
        }
    }
}
