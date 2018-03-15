#pragma once
#include <d3d12.h>
#include "common.h"
#include <vector>
//CB: Create descriptorHeap, 

//Device (from D3DFactory needed for createCommitedResource, 

class BezierClass
{
public:
	//public functions
	//static void SetUploadHeap(ID3D12Resource* pUploadHeap);

	BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource** ppUploadHeap, ID3D12Resource** ppConstantHeap, 
		unsigned char** ppBufferAddressPointer, int iNrOfPoints, float4* pBezierPoints, double* pPointsOffset, unsigned int iBufferCount);
	~BezierClass();

	void UpdateBezierPoints(ID3D12GraphicsCommandList* pCopyCL, double deltaTime, int iBufferIndex);
	void BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);
	void UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);

private:
	//private functions
	//static ID3D12Resource*				s_pUploadHeap;

	//private variables
	float4*								m_pBezierPoints;
	double*								m_pDeltaTimePoints;
	int									m_iNrOfPoint;
	unsigned char**						m_ppBufferAddressPointer;

	D3D12_RESOURCE_BARRIER*				m_pTransitionToConstant;
	D3D12_RESOURCE_BARRIER*				m_pTransitionToCopyDest;

	ID3D12DescriptorHeap*				m_pConstantDescHeap;
	ID3D12Resource**					m_ppUploadHeap;
	ID3D12Resource**					m_ppConstantBuffer;

	unsigned int _iBufferCount;
};