struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

float4 main(VS_OUT input) : SV_TARGET
{
	return input.Col;
}
