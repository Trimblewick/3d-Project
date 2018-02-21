#include "stdafx.h"
#include "BezierClass.h"

//TODO: move root constants/paramters etc to factory, 
//		make a uploadHeap function in D3DFactory,
//		make bonstantCuffer function in D3DFactory, which takes a vector<float4> and links to ComputeShader
//		createRS in D3DFactory

BezierClass::BezierClass(/*D3DFactory* pFactory*/)
{
	//m_pFactory = pFactory;
}

BezierClass::~BezierClass()
{
	m_pBezierVertices.clear();
	//SAFE_RELEASE(m_pFactory);
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

	//------------------------------------------------move all above line to D3DFactory

	CalculateBezierVertices();
	//m_pFactory->SetCBData(m_pBezierVertices); //send m_pBezierVertices to D3DFactory to be send with bonstantCuffer

	m_pBezierVertices.clear();
	return;
}

void BezierClass::CalculateBezierVertices()
{
	//Calculate bézier vertices 

	//m_pBezierVertices.push_back(calculated vertices)
	
	return;
}
