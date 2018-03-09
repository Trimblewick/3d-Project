#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <assert.h>
#include "common.h"
#include <vector>
//CB: Create descriptorHeap, 

//Device (from D3DFactory needed for createCommitedResource, 

class BezierClass
{
public:
	//public functions
	BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource* pResource, uint8_t* address, int iNrOfPoints, float4* pBezierPoints, double* pPointsOffset);
	~BezierClass();

	void UpdateBezierPoints(double deltaTime);
	void BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);
	void UnbindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);

private:
	//private functions


	//private variables
	float4*								m_pBezierPoints;
	double*								m_pDeltaTimePoints;
	int									m_iNrOfPoint;
	uint8_t*							m_address;

	ID3D12DescriptorHeap*				m_pConstantDescHeap;
	ID3D12Resource*						m_pConstantUploadHeap;
};