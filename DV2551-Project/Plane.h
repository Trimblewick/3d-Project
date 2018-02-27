#pragma once
#include <vector>
#include <d3d12.h>

typedef union {
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
} float4;

class Plane
{
public:
	Plane(unsigned int tileSize, ID3D12Resource* pVBuffer, D3D12_VERTEX_BUFFER_VIEW vertexBufferView, ID3D12Resource* pIBuffer, D3D12_INDEX_BUFFER_VIEW indexBufferView);
	~Plane();

	void bind(ID3D12GraphicsCommandList* pCL);

private:
	unsigned int					m_uiSize;
	std::vector<float4>				m_pVerts;

	ID3D12DescriptorHeap*			m_pDHverts;
	D3D12_ROOT_DESCRIPTOR_TABLE*	m_pDescriptorTable;

	ID3D12Resource*					m_pVertexBuffer;
	ID3D12Resource*					m_pIndexBuffer;

	D3D12_VERTEX_BUFFER_VIEW		m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_indexBufferView;
};