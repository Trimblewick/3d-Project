
float4 main( uint id : SV_VERTEXID/*float4 pos : POSITION*/ ) : SV_POSITION
{
	if (id == 0)
	{
		return float4(-0.5f, -0.5f, 0.0, 1.0f);
	}
	if (id == 1)
	{
		return float4(0.0f, -0.5f, 0.0, 1.0f);
	}
	return float4(0.5f, -0.5f, 0.0, 1.0f);
}