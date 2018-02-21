#pragma once
#include <vector>
#include <d3d12.h>

struct float4
{
	float x, y, z, w;
};

class Plane
{
public:
	Plane(unsigned int size, ID3D12DescriptorHeap* pDHverts, ID3D12Resource* pCBUpload, D3D12_ROOT_DESCRIPTOR_TABLE* pDescriptorTable);
	~Plane();

	void bind(ID3D12GraphicsCommandList* pCL, int iParameterIndex);

private:
	unsigned int					m_uiSize;
	std::vector<float4>				m_ppVerts;

	ID3D12DescriptorHeap*			m_pDHverts;
	ID3D12Resource*					m_pCBUpload;
	D3D12_ROOT_DESCRIPTOR_TABLE*	m_pDescriptorTable;
};