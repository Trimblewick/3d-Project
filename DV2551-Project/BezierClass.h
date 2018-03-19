#pragma once
#include <d3d12.h>
#include "common.h"
#include <vector>
//CB: Create descriptorHeap, 

//Device (from D3DFactory needed for createCommitedResource, 

class BezierClass
{
public:
	static void SetUploadHeaps(ID3D12Resource** ppUploadHeap);
	static ID3D12Resource** GetUploadHeaps();

	BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource*** pppUploadHeap, ID3D12Resource** ppConstantHeap, 
		unsigned char*** pppBufferAddressPointer, int iNrOfPoints, float4* pBezierPoints, double* pPointsTimeOffset, unsigned int iBufferCount);
	~BezierClass();

	void UpdateBezierPoints(ID3D12GraphicsCommandList* pCopyCL, double deltaTime, int iBufferIndex);
	void BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);
	void UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);

private:
	static ID3D12Resource**				s_ppUploadHeaps;


	float4*								m_pBezierPoints;
	double*								m_pDeltaTimePoints;
	int									m_iNrOfPoint;
	int									m_iSize;
	unsigned char***					m_pppBufferAddressPointer;
	bool								m_bPingPong;

	D3D12_RESOURCE_BARRIER*				m_pTransitionToConstant;
	D3D12_RESOURCE_BARRIER*				m_pTransitionToCopyDest;

	D3D12_RANGE							m_rangeInHeap;

	ID3D12DescriptorHeap*				m_pConstantDescHeap;
	ID3D12Resource***					m_pppUploadHeap;
	ID3D12Resource**					m_ppConstantBuffer;

	unsigned int _iBufferCount;
};