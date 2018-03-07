#include "stdafx.h"
#include "Plane.h"

Plane::Plane(unsigned int tiles, ID3D12Resource * pVBuffer, D3D12_VERTEX_BUFFER_VIEW vertexBufferView, ID3D12Resource * pIBuffer, D3D12_INDEX_BUFFER_VIEW indexBufferView)
{
	m_uiWidth = tiles + 1;
	m_pVertexBuffer = pVBuffer;
	m_vertexBufferView = vertexBufferView;
	m_pIndexBuffer = pIBuffer;
	m_indexBufferView = indexBufferView;
	m_uiNumIndices = m_indexBufferView.SizeInBytes / sizeof(DWORD);
}

Plane::~Plane()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

void Plane::SetVertexBuffer(ID3D12Resource * pVBuffer)
{
	m_pVertexBuffer = pVBuffer;
}

void Plane::SetVertexBufferView(D3D12_VERTEX_BUFFER_VIEW vertexBufferView)
{
	m_vertexBufferView = vertexBufferView;
}

void Plane::SetIndexBuffer(ID3D12Resource * pIBuffer)
{
	m_pIndexBuffer = pIBuffer;
}

void Plane::SetIndexBufferView(D3D12_INDEX_BUFFER_VIEW indexBufferView)
{
	m_indexBufferView = indexBufferView;
}

void Plane::bind(ID3D12GraphicsCommandList* pCL)
{
	pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCL->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	pCL->IASetIndexBuffer(&m_indexBufferView);
	pCL->DrawIndexedInstanced(m_uiNumIndices, 1, 0, 0, 0);
}

std::vector<float2>* Plane::GetVertices()
{
	return &m_pVerts;
}

std::vector<DWORD>* Plane::GetIndices()
{
	return &m_pIndices;
}

unsigned int Plane::GetWidth()
{
	return m_uiWidth;
}
