#include "stdafx.h"
#include "Plane.h"

Plane::Plane(unsigned int size, ID3D12DescriptorHeap* pDHverts, ID3D12Resource* pCBUpload, D3D12_ROOT_DESCRIPTOR_TABLE* pDescriptorTable)
{
	m_uiSize = size;
	m_ppVerts.reserve(size * size);
	m_pDHverts = pDHverts;
	m_pCBUpload = pCBUpload;
	m_pDescriptorTable = pDescriptorTable;

	for (unsigned int i = 0; i < size; ++i)
	{
		for (unsigned int j = 0; j < size; ++j)
		{
			m_ppVerts.push_back(float4{ i, j, 0, 1 });
		}
	}

	
}

Plane::~Plane()
{
	SAFE_RELEASE(m_pDHverts);
	SAFE_RELEASE(m_pCBUpload);
}

void Plane::bind(ID3D12GraphicsCommandList* pCL, int iParameterIndex)
{
	pCL->SetGraphicsRootShaderResourceView(iParameterIndex, m_pCBUpload->GetGPUVirtualAddress());
}
