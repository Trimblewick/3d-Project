#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <assert.h>

//CB: Create descriptorHeap, 

//Device (from D3DFactory needed for createCommitedResource, 
struct float4
{
	float x;
	float y;
	float z;
	float w;
};

class BezierClass
{
public:
	//public functions
	BezierClass();
	BezierClass(ID3D12DescriptorHeap* pDH, ID3D12Resource* pResource);
	~BezierClass();
	void CalculateBezierVertices();
	void BindBezier(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);

private:
	//private functions
	

	//private variables
	std::vector<float4>					m_pBezierVertices;
	std::vector<float4>					m_pPreviouslyCalculatedBezierVertices;
	int									m_nrOfVertices;

	ID3D12DescriptorHeap*				m_pConstantDescHeap;
	ID3D12Resource*						m_pConstantUploadHeap;	
};