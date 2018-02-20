#include "stdafx.h"
#include "BezierClass.h"

//TODO: move root constants/paramters etc to factory, 
//		make a uploadHeap function in D3DFactory,
//		

BezierClass::BezierClass()
{
	//interact with D3DFactory
}

BezierClass::~BezierClass()
{
	//DELET THIS, hihi
}

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

	float4 color;
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
	m_pBezierVertices.clear();
	return;
}
