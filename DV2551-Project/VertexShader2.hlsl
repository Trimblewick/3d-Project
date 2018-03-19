
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

cbuffer bezierOffset : register(b2)
{
	int2 offset;
};

float4 BezierCurve(float4 p0, float4 p1, float4 p2, float4 p3, float u)
{
	float l0 = (1 - u) * (1 - u) * (1 - u);
	float l1 = 3 * u * (1 - u) * (1 - u);
	float l2 = 3 * u * u * (1 - u);
	float l3 = u * u * u;
	return p0 * l0 + p1 * l1 + p2 * l2 + p3 * l3;
}


float4 main(float2 input : UV) : SV_POSITION
{
	float4 points[4];
	for (int i = 0; i < 4; ++i)
	{
		points[i] = BezierCurve(bezierVertices[i], bezierVertices[i + 4], bezierVertices[i + 8], bezierVertices[i + 12], input.x);
	}
	float4 vert = BezierCurve(points[0], points[1], points[2], points[3], input.y);
	vert.x += offset.x;
	vert.z += offset.y;

	return mul(vert, vpMat);
}
