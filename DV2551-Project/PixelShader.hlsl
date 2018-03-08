float4 main(float4 input : SV_POSITION) : SV_TARGET
{	
	return float4(input.x, 0.f, 0.f, 1.f) / 1000; //input.color;
}
