#include "stdafx.h"
#include "BezierClass.h"
#include <time.h>
#include <stdlib.h>

BezierClass::BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource* pResource, uint8_t* address, int iNrOfPoints, float4* pBezierPoints, double* pPointsOffset)
{
	m_iNrOfPoint = iNrOfPoints;
	m_pConstantDescHeap = pDH;
	m_pConstantUploadHeap = pResource;
	m_address = address;
	m_pDeltaTimePoints = pPointsOffset;
	m_pBezierPoints = pBezierPoints;
}


BezierClass::~BezierClass()
{
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pConstantUploadHeap);
	delete[] m_pBezierPoints;
	delete[] m_pDeltaTimePoints;
}


void BezierClass::UpdateBezierPoints(double deltaTime)
{
	for (int i = 0; i < m_iNrOfPoint; ++i)
	{
		m_pDeltaTimePoints[i] += deltaTime;

		m_pBezierPoints[i].y = sin(m_pDeltaTimePoints[i]);
	}

	memcpy(m_address, m_pBezierPoints, m_iNrOfPoint * sizeof(float4));
}


void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->SetGraphicsRootConstantBufferView(1, m_pConstantUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}

void BezierClass::UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	//UNbind
}