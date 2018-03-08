#include "stdafx.h"
#include "BezierClass.h"
#include <time.h>
#include <stdlib.h>
	

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
	delete[] m_pDeltaTime;
}


void BezierClass::CalculateBezierPoints(int width, int x, int z)
{
	std::srand(time(NULL));
	float grid = width / 3.0f;

	for (int x = 0; x < 16; ++x)
	{
		m_pDeltaTime[x] = rand() % 20 - 10;
	}

	//Do for every patch
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			float4 temp;
			temp.x = grid*j + width * x;
			temp.y = sin(m_pDeltaTime[j + i*4]);
			temp.z = grid * i + width * z;
			temp.w = 1.0f;

			m_pBezierPoints.push_back(temp);
		}
	}

	memcpy(m_address, m_pBezierPoints.data(), m_nrOfVertices * sizeof(float4));
}


void BezierClass::UpdateBezierPoints(double deltaTime, int x, int z, int patchWidth)
{
	for (int i = 0; i < m_pBezierPoints.size() - 1; ++i)
	{
		m_pDeltaTime[i] += deltaTime;

		m_pBezierPoints[i].y = sin(m_pDeltaTime[i]);
	}

	memcpy(m_address, m_pBezierPoints.data(), m_nrOfVertices * sizeof(float4));
}


void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->SetGraphicsRootConstantBufferView(1, m_pConstantUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}
