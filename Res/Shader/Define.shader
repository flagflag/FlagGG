uniform mat4 _WorldMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _ProjViewMatrix;
uniform mat4 _SkinMatrices[64];
uniform mat4 _LightViewMatrix;
uniform mat4 _LightProjViewMatrix;

uniform vec4 _DeltaTimeT;
uniform vec4 _ElapsedTimeT;
uniform vec4 _CameraPosT;
uniform vec4 _LightPosT;
uniform vec4 _LightDirT;

uniform vec4 _LightPositions[10];
uniform vec4 _LightColors[10];

SAMPLER2D(_DiffuseMap, 1);
SAMPLER2D(_NormalMap, 2);
SAMPLER2D(_SpecularMap, 3);
SAMPLER2D(_EmissiveMap, 4);
SAMPLER2D(_EnvMap, 5);
SAMPLER2D(_ShadowMap, 6);

#define _DeltaTime _DeltaTimeT.x
#define _ElapsedTime _ElapsedTimeT.x
#define _CameraPos _CameraPosT.xyz
#define _LightPos _LightPosT.xyz
#define _LightDir _LightDirT.xyz
