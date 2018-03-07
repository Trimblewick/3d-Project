#include "stdafx.h"
#include "BezierClass.h"

//TODO: Quick test bind cbuffer to shader and see if values get through 
//		memcpy new cbuffer data in update loop every frame to cbuffer??
//		m_pBezierVertices used in D3DFactory original memcpy only has 3 vertices
//		

BezierClass::BezierClass(/**/)
{
}

BezierClass::BezierClass(ID3D12DescriptorHeap * pDH, ID3D12Resource* pResource, uint8_t* address, int nrOfVertices)
{
	m_nrOfVertices = nrOfVertices;
	m_pConstantDescHeap = pDH;
	m_pConstantUploadHeap = pResource;
	m_address = address;
}

BezierClass::~BezierClass()
{
	m_pBezierPoints.clear();
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pConstantUploadHeap);
	delete m_address;
	//DELET THIS, hihi
}

void BezierClass::CalculateBezierPoints()
{
	//Change entire function to take nrOfVertices from PLane(), choose a couple of them and offset them in Y
	float4 test;
	test.x = -2.5f;
	test.y = -2.5f;
	test.z = 10.0f;
	test.w = 1.0f;

	float4 test2;
	test2.x = -2.5f;
	test2.y = 2.5f;
	test2.z = 10.0f;
	test2.w = 1.0f;

	float4 test3;
	test3.x = 2.5f;
	test3.y = -2.5f;
	test3.z = 10.0f;
	test3.w = 1.0f;

	m_pBezierPoints.push_back(test);
	m_pBezierPoints.push_back(test2);
	m_pBezierPoints.push_back(test3);

	memcpy(m_address, m_pBezierPoints.data(), m_nrOfVertices * sizeof(float4));

	return;
}

void BezierClass::UpdateBezierPoints()
{
	for (int i = 0; i < m_nrOfVertices; ++i)
	{
		m_pBezierPoints[i] = m_pBezierPoints[i]; //change to a random Y factor, something like comment below this line
		//m_pBezierVertices[i].y = random value between ??? 0 and 10???
	}

	//memcpy(m_address, reinterpret_cast<void*>(&m_pBezierVertices), m_nrOfVertices * sizeof(float4));
}

void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->SetGraphicsRootConstantBufferView(1, m_pConstantUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}
