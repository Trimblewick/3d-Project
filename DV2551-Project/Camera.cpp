#include "stdafx.h"
#include "Camera.h"



Camera::Camera(CameraBuffer data, D3D12_VIEWPORT viewport, D3D12_RECT scissorRect, 
	ID3D12Resource** ppBufferMatrix, unsigned char** ppBufferAdressPointers, unsigned int iBufferCount, ID3D12DescriptorHeap* pDH)
{
	m_data = data;
	m_viewport = viewport;
	m_rectscissor = scissorRect;
	m_iBufferCount = iBufferCount;
	m_ppBufferMatrix = ppBufferMatrix;
	m_ppBufferAdressPointers = ppBufferAdressPointers;
	m_pDH = pDH;
}

Camera::~Camera()
{
	D3D12_RANGE rangeRead = {};
	for (int i = 0; i < m_iBufferCount; ++i)
	{
		m_ppBufferMatrix[i]->Unmap(0, &rangeRead);
		SAFE_RELEASE(m_ppBufferMatrix[i]);
	}
	SAFE_RELEASE(m_pDH);
	delete[] m_ppBufferMatrix;
	delete[] m_ppBufferAdressPointers;
}

void Camera::Update(Input * pInput, double dDeltaTime, unsigned int iBufferIndex)
{
	
}

void Camera::BindCamera(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->RSSetViewports(1, &m_viewport);
	pCL->RSSetScissorRects(1, &m_rectscissor);
	//pCL->SetGraphicsRootConstantBufferView(0, m_ppBufferMatrix[iBufferIndex]->GetGPUVirtualAddress());
}


