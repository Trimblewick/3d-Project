#include "stdafx.h"
#include "BezierClass.h"
#include <time.h>
#include <stdlib.h>


BezierClass::BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource** ppUploadHeap, ID3D12Resource** ppConstantHeap,
	unsigned char** ppBufferAddressPointers, int iNrOfPoints, float4* pBezierPoints, double* pPointsOffset, unsigned int iBufferCount)
{
	//if (s_pUploadHeap == nullptr)
		//OutputDebugString(L"No Upload Heap Availible, need to set one using SetUploadHeap before creating any instances of this class");

	m_iNrOfPoint = iNrOfPoints;
	m_pConstantDescHeap = pDH;
	m_ppUploadHeap = ppUploadHeap;
	m_ppConstantBuffer = ppConstantHeap;
	m_ppBufferAddressPointer = ppBufferAddressPointers;
	m_pDeltaTimePoints = pPointsOffset;
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
		SAFE_RELEASE(m_ppUploadHeap[i]);
		SAFE_RELEASE(m_ppConstantBuffer[i]);
	}
	delete[] m_ppUploadHeap;
	delete[] m_ppConstantBuffer;
	delete[] m_pBezierPoints;
	delete[] m_pDeltaTimePoints;
	delete[] m_ppBufferAddressPointer;
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

	memcpy(m_ppBufferAddressPointer[iBufferIndex], m_pBezierPoints, m_iNrOfPoint * sizeof(float4));
	pCopyCL->CopyResource(m_ppConstantBuffer[iBufferIndex], m_ppUploadHeap[iBufferIndex]);
}


void BezierClass::BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->ResourceBarrier(1, &m_pTransitionToConstant[iBufferIndex]);
	pCL->SetGraphicsRootConstantBufferView(1, m_ppUploadHeap[iBufferIndex]->GetGPUVirtualAddress()); //Bind to shader register 
}

void BezierClass::UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	//UNbind
	pCL->ResourceBarrier(1, &m_pTransitionToCopyDest[iBufferIndex]);
}