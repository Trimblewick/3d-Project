#include "stdafx.h"
#include "BezierClass.h"

//TODO: move root constants/paramters etc to factory, 
//		make a uploadHeap function in D3DFactory,
//		make bonstantCuffer function in D3DFactory, which takes a vector<float4> and links to CS
//		createRS in D3DFactory

BezierClass::BezierClass()
{
	//interact with D3DFactory
}

BezierClass::~BezierClass()
{
	m_pBezierVertices.clear();
	//DELET THIS, hihi
}

//Sends data calculated in BezierClass to D3DFactory to be bound in constantbuffers
void BezierClass::DX12Highway()
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

	//commandList->SetGraphicsRoot32BitConstants(color etc..);
	//commandList->DrawInstanced();

	//createRS()??

	//------------------------------------------------
	CalculateBezierVertices();
	//
	//send m_pBezierVertices to D3DFactory to be send with bonstantCuffer

	m_pBezierVertices.clear();
	return;
}

void BezierClass::CalculateBezierVertices()
{
	//Calculate bézier vertices 

	//m_pBezierVertices.push_back(calculated vertices)
	
	return;
}
