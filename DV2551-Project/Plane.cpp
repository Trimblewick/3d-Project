#include "stdafx.h"
#include "Plane.h"

Plane::Plane(unsigned int size, D3DFactory* pFactory)
{
	m_uiSize = size;
	m_ppVerts.reserve(size * size);
	m_pFactory = pFactory;

	for (unsigned int i = 0; i < size; ++i)
	{
		for (unsigned int j = 0; j < size; ++j)
		{
			m_ppVerts.push_back(float4{ i, j, 0, 1 });
		}
	}

	D3D12_HEAP_PROPERTIES heapProperties = D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT, 
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	D3D12_RESOURCE_DESC desc = D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, m_uiSize * m_uiSize, 1, 1, 1, 
		DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };

	m_pFactory->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVBUpload));

	m_pDHverts = m_pFactory->CreateDH(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
}

Plane::~Plane()
{
	SAFE_RELEASE(m_pDHverts);
	SAFE_RELEASE(m_pVBUpload);
}

void Plane::bind(ID3D12GraphicsCommandList* pCL, int iParameterIndex)
{
	pCL->SetGraphicsRootShaderResourceView(iParameterIndex, m_pVBUpload->GetGPUVirtualAddress());
}
