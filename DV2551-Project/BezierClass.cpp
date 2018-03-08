#include "stdafx.h"
#include "BezierClass.h"
#include <time.h>

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
	//DELET THIS, hihis
}

void BezierClass::CalculateBezierPoints(int width/*, int x, int y*/)
{
	//int patchWidth = sqrt(nrOfPatches);
	float grid = width / 3.0f;

	////For every patch in X
	//for (int x = 0; x < patchWidth; ++x)
	//{
	//	//For every patch in Y
	//	for (int y = 0; y < patchWidth; ++y)
	//	{
			//Do for every patch
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					float4 temp;
					temp.x = grid*j/* + width * x*/;
					temp.y = rand() % 20 - 10;
					temp.z = grid * i/* + width * y*/;
					temp.w = 1.0f;

					m_pBezierPoints.push_back(temp);
				}
			}
	//	}
	//}
	memcpy(m_address, m_pBezierPoints.data(), m_nrOfVertices * sizeof(float4));
}

void BezierClass::UpdateBezierPoints()
{
	for (int i = 0; i < m_pBezierPoints.size(); ++i)
	{
		//m_pBezierPoints[i].y = m_pBezierPoints[i].y + (rand() % 20 - 10)/100.0f; //change to a random Y factor, something like comment below this line
		//m_pBezierPoints[i].y = m_pBezierPoints[i].y;//random value between ??? 0 and 10???
	}

	//memcpy(m_address, m_pBezierPoints.data(), m_nrOfVertices * sizeof(float4));
}

void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->SetGraphicsRootConstantBufferView(1, m_pConstantUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}
