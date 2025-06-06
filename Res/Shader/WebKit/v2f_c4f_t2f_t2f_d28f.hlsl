#include "Shader/Define.hlsl"

cbuffer Uniforms : register(b1)
{
  float4 State;
  float4x4 Transform;
  float4 Scalar4[2];
  float4 Vector[8];
  uint ClipSize;
  float4x4 Clip[8];
};

float4 sRGBToLinear(float4 val) { return float4(val.xyz * (val.xyz * (val.xyz * 0.305306011 + 0.682171111) + 0.012522878), val.w); }

struct VS_OUTPUT
{
  float4 Position    : SV_POSITION;
  float4 Color       : COLOR0;
  float2 TexCoord    : TEXCOORD0;
  float4 Data0       : COLOR1;
  float4 Data1       : COLOR2;
  float4 Data2       : COLOR3;
  float4 Data3       : COLOR4;
  float4 Data4       : COLOR5;
  float4 Data5       : COLOR6;
  float4 Data6       : COLOR7;
  float2 ObjectCoord : TEXCOORD1;
};

VS_OUTPUT VS(float2 Position : POSITION,
             float4 Color    : COLOR0,
             float2 TexCoord : TEXCOORD0,
             float2 ObjCoord : TEXCOORD1,
             float4 Data0    : COLOR1,
			       float4 Data1    : COLOR2,
             float4 Data2    : COLOR3,
             float4 Data3    : COLOR4,
             float4 Data4    : COLOR5,
             float4 Data5    : COLOR6,
             float4 Data6    : COLOR7)
{
  VS_OUTPUT output;
  output.ObjectCoord = ObjCoord;
  // output.Position = mul(Transform, float4(Position, 0.0, 1.0));
  output.Position = float4(mul(float4(Position, 0.0, 1.0), worldMatrix), 1.0);
  output.Color = Color;
  output.TexCoord = TexCoord;
  output.Data0 = Data0;
  output.Data1 = Data1;
  output.Data2 = Data2;
  output.Data3 = Data3;
  output.Data4 = Data4;
  output.Data5 = Data5;
  output.Data6 = Data6;
  return output;
}