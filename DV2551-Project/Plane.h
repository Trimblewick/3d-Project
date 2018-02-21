#pragma once
#include <vector>
#include "D3DFactory.h"
#include <d3d12.h>

struct float4
{
	float x, y, z, w;
};

class Plane
{
public:
	Plane(unsigned int size, D3DFactory* pFactory);
	~Plane();

	void bind(ID3D12GraphicsCommandList* pCL, int iParameterIndex);

private:
	unsigned int m_uiSize;
	std::vector<float4> m_ppVerts;

	ID3D12DescriptorHeap*		m_pDHverts;

	ID3D12Resource*				m_pVBUpload;
	D3DFactory*					m_pFactory;
};