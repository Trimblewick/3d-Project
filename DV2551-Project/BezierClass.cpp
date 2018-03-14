//Get ticks from queue, GetClockCalibration()
//Get queue frequency, GPU and CPU

//Convert queue time to seconds/ms something measurable (clockcalib / frequency)

//Sync CPU time and offset one GPU time accordingaly, EG: Q0(60,55) Q1(90,50) becomes Q0(5,0) Q1(40,0)
//Calculate GPU offset 40 - 5 = 35

//Move measurements to start at 0 for Q0 ??
//Move measurements to start at 35 for Q1 ??

#include "stdafx.h"
#include "BezierClass.h"
#include <time.h>
#include <stdlib.h>

BezierClass::BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource* pUploadHeap, ID3D12Resource* pConstantHeap, uint8_t* address, int iNrOfPoints, float4* pBezierPoints, double* pPointsOffset)
{
	m_iNrOfPoint = iNrOfPoints;
	m_pConstantDescHeap = pDH;
	m_pUploadHeap = pUploadHeap;
	m_pConstantBuffer = pConstantHeap;
	m_address = address;
	m_pDeltaTimePoints = pPointsOffset;
	m_pBezierPoints = pBezierPoints;

	D3D12_RESOURCE_TRANSITION_BARRIER transitionTo = {};
	transitionTo.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	transitionTo.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	transitionTo.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	transitionTo.pResource = pConstantHeap;

	D3D12_RESOURCE_TRANSITION_BARRIER transitionFrom = {};
	transitionFrom.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	transitionFrom.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	transitionFrom.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	transitionFrom.pResource = pConstantHeap;

	m_transitionToConstant.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_transitionToConstant.Transition = transitionTo;
	m_transitionToConstant.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;

	m_transitionToCopyDest.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_transitionToCopyDest.Transition = transitionFrom;
	m_transitionToCopyDest.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
}


BezierClass::~BezierClass()
{
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pUploadHeap);
	SAFE_RELEASE(m_pConstantBuffer);
	delete[] m_pBezierPoints;
	delete[] m_pDeltaTimePoints;
}


void BezierClass::UpdateBezierPoints(ID3D12GraphicsCommandList* pCopyCL, double deltaTime)
{
	for (int i = 0; i < m_iNrOfPoint; ++i)
	{
		m_pDeltaTimePoints[i] += deltaTime;

		m_pBezierPoints[i].y = sin(m_pDeltaTimePoints[i]);
	}

	memcpy(m_address, m_pBezierPoints, m_iNrOfPoint * sizeof(float4));
	pCopyCL->CopyResource(m_pConstantBuffer, m_pUploadHeap);
}


void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->ResourceBarrier(1, &m_transitionToConstant);
	pCL->SetGraphicsRootConstantBufferView(1, m_pUploadHeap->GetGPUVirtualAddress()); //Bind to shader register 
}

void BezierClass::UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	//UNbind
	pCL->ResourceBarrier(1, &m_transitionToCopyDest);
}