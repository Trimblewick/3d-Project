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

struct VS_IN
{
	float4 position : SV_POSITION;
};

float4 main(float4 input : SV_POSITION) : SV_POSITION
{
	return mul(input, vpMat);
}
