cbuffer camera : register(b0)
{
	float4x4 vpMat;
	float4x4 viewMat;
	float4x4 projMat;
	float3   position;
	float pad;
	float3	forward;
	float pad2;
	float3	right;
	float pad3;
	float3	up;
	float pad4;
};

cbuffer bezier : register(b1)
{
	float4 bezierVertices[16];
}

float4 main( uint id : SV_VERTEXID/*float4 pos : POSITION*/ ) : SV_POSITION
{	
	//if (id == 0 && bezierVertices[5].x > 5.0f && bezierVertices[5].x < 6.0f && bezierVertices[5].z > 5.0f && bezierVertices[5].z < 6.0f)
	//{
	//	return mul(float4(-2.0f, 0.5f, 10.0, 1.0f), vpMat);
	//	//return mul(float4(-5.0f, 5.0f, 10.0, 1.0f), vpMat);
	//	//return mul(bezierVertices[0], vpMat);
	//}
	if (id == 0)
	{
		return mul(float4(0.5f, 0.5f, 10.0, 1.0f), vpMat);
		//return mul(float4(-5.0f, 5.0f, 10.0, 1.0f), vpMat);
		//return mul(bezierVertices[0], vpMat);
	}
	if (id == 1)
	{
		return mul(float4(0.0f, -2.5f, 10.0, 1.0f), vpMat);
		//return mul(bezierVertices[1], vpMat);
	}
	else
	{
		return mul(float4(0.5f, 0.5f, 5.0, 1.0f), vpMat);
		//return mul(bezierVertices[2], vpMat);
	}
	//return mul(bezierVertices[id], vpMat);
}
