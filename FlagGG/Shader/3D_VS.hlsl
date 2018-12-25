struct VertexType
{
	float3 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 nor : NORMAL;
};
 
VertexType VS( VertexType input )
{
	return input;
}

