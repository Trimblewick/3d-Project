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
	BezierClass();
	BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource* pResource, uint8_t* address, int nrOfVertices);
	~BezierClass();
	void CalculateBezierPoints(int width);
	void UpdateBezierPoints();
	void BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);

private:
	//private functions
	

	//private variables
	std::vector<float4>					m_pBezierPoints;
	std::vector<float4>					m_pPlaneVertices;
	int									m_nrOfVertices;
	uint8_t*							m_address;

	ID3D12DescriptorHeap*				m_pConstantDescHeap;
	ID3D12Resource*						m_pConstantUploadHeap;	
};