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
	m_pBezierVertices.clear();
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pConstantUploadHeap);
	//DELET THIS, hihi
}

void BezierClass::CalculateBezierVertices()
{

	//Change entire function to take vector containing vertices from Plane() and offset Y value randomly then pushback and memcpy
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

	memcpy(m_address, reinterpret_cast<void*>(&m_pBezierVertices), m_nrOfVertices * sizeof(float4));

	return;
}

void BezierClass::UpdateBezierVertices()
{
	for (int i = 0; i < m_nrOfVertices; ++i)
	{
		m_pBezierVertices[i] = m_pBezierVertices[i]; //change to a random Y factor, something like comment below this line
		//m_pBezierVertices[i].y = random value between ??? 0 and 10???
	}

	memcpy(m_address, reinterpret_cast<void*>(&m_pBezierVertices), m_nrOfVertices * sizeof(float4));
}

void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->SetGraphicsRootConstantBufferView(1, m_pConstantUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}
