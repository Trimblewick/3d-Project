#include "stdafx.h"
#include "Plane.h"

Plane::Plane(unsigned int tiles)
{
	m_uiWidth = tiles + 1;
	m_pVerts.reserve(m_uiWidth * m_uiWidth);

	for (unsigned int i = 0; i < m_uiWidth; ++i)
	{
		for (unsigned int j = 0; j < m_uiWidth; ++j)
		{
			m_pVerts.push_back(float4{ (float)j, 0, (float)i, 1 });
		}
	}
	unsigned int uiNumIndices = (m_uiWidth - 1)* (m_uiWidth - 1);
	m_pIndices.reserve(uiNumIndices);

	for (unsigned int i = 0; i < (m_uiWidth - 1); ++i)
	{
		for (unsigned int j = 0; j < (m_uiWidth - 1); ++j)
		{
			m_pIndices.push_back(i * m_uiWidth + j);
			m_pIndices.push_back(i * m_uiWidth + j + m_uiWidth);
			m_pIndices.push_back(i * m_uiWidth + j + m_uiWidth + 1);
			m_pIndices.push_back(i * m_uiWidth + j);
			m_pIndices.push_back(i * m_uiWidth + j + m_uiWidth + 1);
			m_pIndices.push_back(i * m_uiWidth + j + 1);
		}
	}
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
	pCL->DrawIndexedInstanced(m_pIndices.size(), 1, 0, 0, 0);
}

std::vector<float4>* Plane::GetVertices()
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
