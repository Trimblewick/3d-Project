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

std::vector<ID3D12Resource**>				BezierClass::s_pppUploadHeaps;

void BezierClass::AddUploadHeap(ID3D12Resource ** ppUploadHeap)
{
	s_pppUploadHeaps.push_back(ppUploadHeap);
}

ID3D12Resource ** BezierClass::GetLastUploadHeap()
{
	if (s_pppUploadHeaps.size() > 0)
		return s_pppUploadHeaps.back();
}

BezierClass::BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource*** pppUploadHeap, ID3D12Resource** ppConstantHeap,
	unsigned char*** pppBufferAddressPointers, int iNrOfPoints, float4* pBezierPoints, double* pPointsTimeOffset, unsigned int iBufferCount)
{
	//if (s_pUploadHeap == nullptr)
		//OutputDebugString(L"No Upload Heap Availible, need to set one using SetUploadHeap before creating any instances of this class");

	m_iNrOfPoint = iNrOfPoints;
	m_iSize = m_iNrOfPoint * sizeof(float4);
	m_pConstantDescHeap = pDH;
	m_pppUploadHeap = pppUploadHeap;
	m_ppConstantBuffer = ppConstantHeap;
	m_pppBufferAddressPointer = pppBufferAddressPointers;
	m_bPingPong = false;
	m_pDeltaTimePoints = pPointsTimeOffset;
	m_pBezierPoints = pBezierPoints;


	m_pTransitionToConstant = new D3D12_RESOURCE_BARRIER[iBufferCount];
	m_pTransitionToCopyDest = new D3D12_RESOURCE_BARRIER[iBufferCount];

	D3D12_RESOURCE_TRANSITION_BARRIER transitionTo = {};
	transitionTo.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	transitionTo.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	transitionTo.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	D3D12_RESOURCE_TRANSITION_BARRIER transitionFrom = {};
	transitionFrom.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	transitionFrom.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	transitionFrom.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	
	_iBufferCount = iBufferCount;
	for (int i = 0; i < iBufferCount; ++i)
	{
		transitionTo.pResource = ppConstantHeap[i];		
		transitionFrom.pResource = ppConstantHeap[i];

		m_pTransitionToConstant[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		m_pTransitionToConstant[i].Transition = transitionTo;
		m_pTransitionToConstant[i].Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;

		m_pTransitionToCopyDest[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		m_pTransitionToCopyDest[i].Transition = transitionFrom;
		m_pTransitionToCopyDest[i].Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	}
}


BezierClass::~BezierClass()
{
	SAFE_RELEASE(m_pConstantDescHeap);
	for (int i = 0; i < _iBufferCount; ++i)
	{
		SAFE_RELEASE(m_pppUploadHeap[0][i]);
		SAFE_RELEASE(m_pppUploadHeap[1][i]);
		SAFE_RELEASE(m_ppConstantBuffer[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		delete[] m_pppUploadHeap[i];
		delete[] m_pppBufferAddressPointer[i];
	}
	delete[] m_pppUploadHeap;
	delete[] m_pppBufferAddressPointer;
	delete[] m_ppConstantBuffer;
	delete[] m_pBezierPoints;
	delete[] m_pDeltaTimePoints;
	delete[] m_pTransitionToConstant;
	delete[] m_pTransitionToCopyDest;
}


void BezierClass::UpdateBezierPoints(ID3D12GraphicsCommandList* pCopyCL, double deltaTime, int iBufferIndex)
{
	for (int i = 0; i < m_iNrOfPoint; ++i)
	{
		m_pDeltaTimePoints[i] += deltaTime;

		m_pBezierPoints[i].y = sin(m_pDeltaTimePoints[i]);
	}
	int k = 0;// (int)m_bPingPong;
	bool test = !m_bPingPong;

	s_pppUploadHeaps[(int)(m_rangeInHeap.Begin / 65536)][k][iBufferIndex];

	memcpy(m_pppBufferAddressPointer[k][iBufferIndex], m_pBezierPoints, m_iSize);
	
	pCopyCL->CopyResource(m_ppConstantBuffer[iBufferIndex], m_pppUploadHeap[k][iBufferIndex]);
	//pCopyCL->CopyBufferRegion(m_ppConstantBuffer[iBufferIndex], 0, m_pppUploadHeap[iBufferIndex][k], m_rangeInHeap.Begin, m_iSize);
	m_bPingPong = !m_bPingPong;
}


void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->ResourceBarrier(1, &m_pTransitionToConstant[iBufferIndex]);
	pCL->SetGraphicsRootConstantBufferView(1, m_ppConstantBuffer[iBufferIndex]->GetGPUVirtualAddress()); //Bind to shader register 
}

void BezierClass::UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	//UNbind
	pCL->ResourceBarrier(1, &m_pTransitionToCopyDest[iBufferIndex]);
}