struct VSOUT
{
	float4 pos: SV_POSITION;
	float4 color : COLOR;
};

float4 main(/*VSOUT input*/) : SV_TARGET
{
	
	return float4(1.0f, 1.0f, 1.0f, 1.0f); //input.color;
}
