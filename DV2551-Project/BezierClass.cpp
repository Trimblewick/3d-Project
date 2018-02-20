#include "stdafx.h"
#include "BezierClass.h"

void BezierClass::SetRootDesc()
{
	//fill root desc, create input and register spaces etc..

	//Create one rp for the color root constant
	D3D12_ROOT_PARAMETER rp[1];

	//Color
	D3D12_ROOT_CONSTANTS colorConstant;
	colorConstant.Num32BitValues = 4;
	colorConstant.ShaderRegister = 0; //set color as b0?
	colorConstant.RegisterSpace = 0;

	rp[0].Constants = colorConstant;
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	float4 color;// = float4(1, 0.0f, 0.0f, 1.0f);
	color.x = 1.0f;
	color.y = 0.0f;
	color.z = 0.0f;
	color.w = 1.0f;

	//commandList->SetGraphicsRoot32BitConstants(color etc..);
	//commandList->DrawInstanced();

	//createRS()??

}

void BezierClass::CalculateBezierVertices()
{
	return;
}
