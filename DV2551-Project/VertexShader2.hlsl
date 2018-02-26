struct VS_IN
{
	float4 Pos : POSITION;
	float4 Col : COLOR;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

VS_OUT main(VS_IN input)
{
	return input;
}
