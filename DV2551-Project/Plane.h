#pragma once
#include <vector>
#include <d3d12.h>
#include "common.h"

class Plane
{
public:
	Plane(unsigned int tileSize);
	~Plane();

	void SetVertexBuffer(ID3D12Resource* pVBuffer);
	void SetVertexBufferView(D3D12_VERTEX_BUFFER_VIEW vertexBufferView);
	void SetIndexBuffer(ID3D12Resource* pIBuffer);
	void SetIndexBufferView(D3D12_INDEX_BUFFER_VIEW indexBufferView);

	void bind(ID3D12GraphicsCommandList* pCL);

	std::vector<float4>* GetVertices();
	std::vector<DWORD>* GetIndices();
	unsigned int GetWidth();
private:
	unsigned int					m_uiWidth;
	std::vector<float4>				m_pVerts;
	std::vector<DWORD>				m_pIndices;

	ID3D12Resource*					m_pVertexBuffer;
	ID3D12Resource*					m_pIndexBuffer;

	D3D12_VERTEX_BUFFER_VIEW		m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_indexBufferView;
};