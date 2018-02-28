struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

float4 main() : SV_TARGET
{
	return float4(1, 1, 1, 1);
}
