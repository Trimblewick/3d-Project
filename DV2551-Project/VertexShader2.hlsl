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
};


float4 main(float2 input : UV) : SV_POSITION
{
	return mul(float4(input.x, 0, input.y, 1), vpMat);
}
