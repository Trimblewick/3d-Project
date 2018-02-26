#include "stdafx.h"
#include "Plane.h"

Plane::Plane(unsigned int tileSize, ID3D12Resource * pVBuffer, D3D12_VERTEX_BUFFER_VIEW vertexBufferView)
{
	m_uiSize = tileSize;
	m_pVerts.reserve(m_uiSize * m_uiSize);
	m_pVBuffer = pVBuffer;
	m_vertexBufferView = vertexBufferView;

	for (unsigned int i = 0; i < m_uiSize; ++i)
	{
		for (unsigned int j = 0; j < m_uiSize; ++j)
		{
			m_pVerts.push_back(float4{ (float)i, (float)j, 0, 1 });
		}
	}

}

Plane::~Plane()
{
}

void Plane::bind(ID3D12GraphicsCommandList* pCL)
{
	pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCL->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	pCL->DrawInstanced(3, 1, 0, 0);
}
