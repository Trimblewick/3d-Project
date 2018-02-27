#include "stdafx.h"
#include "BezierClass.h"

//TODO: Quick test bind cbuffer to shader and see if values get through 
//		memcpy new cbuffer data in update loop every frame to cbuffer??
//		m_pBezierVertices used in D3DFactory original memcpy only has 3 vertices
//		

BezierClass::BezierClass(/**/)
{
}

BezierClass::BezierClass(ID3D12DescriptorHeap * pDH, ID3D12Resource* pResource)
{
	float4 temp;
	temp.x = 1.0f;
	temp.y = 0.0f;
	temp.z = 0.0f;
	temp.w = 0.0f;

	m_nrOfVertices = 3; //update

	m_pBezierVertices.push_back(temp);
	m_pPreviouslyCalculatedBezierVertices.push_back(temp);

	m_pConstantDescHeap = pDH;
	m_pConstantUploadHeap = pResource;
}

BezierClass::~BezierClass()
{
	m_pBezierVertices.clear();
	m_pPreviouslyCalculatedBezierVertices.clear();
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pConstantUploadHeap);
	//DELET THIS, hihi
}

void BezierClass::CalculateBezierVertices(/*&commandList*/)
{
	//for (int i = 0; i < m_nrOfVertices; i++)
	//{
	//	//Change line below to actual Bézier calculationusing previous frame's bezier point
	//	float4 previousBezierWithOffset = m_pPreviouslyCalculatedBezierVertices[i]; //previousBezierWithOffset will be m_pPreviouslyCalculatedBezierVertices[i] with an offset

	//	m_pBezierVertices.push_back(previousBezierWithOffset);
	//	m_pPreviouslyCalculatedBezierVertices[i] = previousBezierWithOffset; //update for next frame
	//}

	//m_pPreviouslyCalculatedBezierVertices = m_pBezierVertices;

	float4 test;
	test.x = -5.0f;
	test.y = 0.0f;
	test.z = 10.0f;
	test.w = 1.0f;

	float4 test2;
	test2.x = 0.0f;
	test2.y = 5.0f;
	test2.z = 10.0f;
	test2.w = 1.0f;

	float4 test3;
	test3.x = 5.0f;
	test3.y = 0.0f;
	test3.z = 10.0f;
	test3.w = 1.0f;

	m_pBezierVertices.push_back(test);
	m_pBezierVertices.push_back(test2);
	m_pBezierVertices.push_back(test3);

	return;
}

void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	//Are these two needed? In that case send them as parameters from GameClass
	//pCL->RSSetViewports(1, &m_viewport);
	//pCL->RSSetScissorRects(1, &m_rectscissor);

	//memcpy every frame? How do we update constantUploadHeap for command list otherwise with newly calculated values?
	D3D12_RANGE range = { 0,0 }; //Entire range

	uint8_t* address;
	m_pConstantUploadHeap->Map(0, &range, reinterpret_cast<void**>(&address));
	memcpy(address, reinterpret_cast<void*>(&m_pBezierVertices), m_nrOfVertices * sizeof(float4));
	m_pConstantUploadHeap->Unmap(0, &range);

	pCL->SetGraphicsRootConstantBufferView(1, m_pConstantUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}
