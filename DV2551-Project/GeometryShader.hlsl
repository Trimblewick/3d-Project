struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 nor : NORMAL;
};

[maxvertexcount(3)]
void main(triangle float4 input[3] : SV_POSITION, inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output;

	float4 vec1 = input[1] - input[0];
	float4 vec2 = input[2] - input[0];

	output.nor = normalize(float4(cross(vec1.xyz, vec2.xyz), 0.f));


	for (int i = 0; i < 3; ++i)
	{
		output.pos = input[i];
		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();
}