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
	for (unsigned int i = 0; i < m_iBufferCount; ++i)
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
	DirectX::XMMATRIX worldMatrix;

	if (pInput->IsKeyDown(pInput->W))
	{
		m_data.position.x += m_data.forward.x * dDeltaTime;
		m_data.position.y += m_data.forward.y * dDeltaTime;
		m_data.position.z += m_data.forward.z * dDeltaTime;
	}
	if (pInput->IsKeyDown(pInput->S))
	{
		m_data.position.x -= m_data.forward.x * dDeltaTime;
		m_data.position.y -= m_data.forward.y * dDeltaTime;
		m_data.position.z -= m_data.forward.z * dDeltaTime;
	}
	if (pInput->IsKeyDown(pInput->A))
	{
		m_data.position.x -= m_data.right.x * dDeltaTime;
		m_data.position.y -= m_data.right.y * dDeltaTime;
		m_data.position.z -= m_data.right.z * dDeltaTime;
	}
	if (pInput->IsKeyDown(pInput->D))
	{
		m_data.position.x += m_data.right.x * dDeltaTime;
		m_data.position.y += m_data.right.y * dDeltaTime;
		m_data.position.z += m_data.right.z * dDeltaTime;
	}
	if (pInput->IsKeyDown(pInput->UP_ARROW))
	{
		m_data.position.x += m_data.up.x * dDeltaTime;
		m_data.position.y += m_data.up.y * dDeltaTime;
		m_data.position.z += m_data.up.z * dDeltaTime;
	}
	if (pInput->IsKeyDown(pInput->DOWN_ARROW))
	{
		m_data.position.x -= m_data.up.x * dDeltaTime;
		m_data.position.y -= m_data.up.y * dDeltaTime;
		m_data.position.z -= m_data.up.z * dDeltaTime;
	}

	m_data.viewMat = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_data.position), DirectX::XMLoadFloat3(&m_data.forward), DirectX::XMLoadFloat3(&m_data.up)));
	m_data.vpMat = DirectX::XMMatrixMultiply(m_data.projMat, m_data.viewMat);

	memcpy(m_ppBufferAdressPointers[iBufferIndex], &m_data, sizeof(CameraBuffer));
}

void Camera::BindCamera(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex)
{
	pCL->RSSetViewports(1, &m_viewport);
	pCL->RSSetScissorRects(1, &m_rectscissor);
	pCL->SetGraphicsRootConstantBufferView(0, m_ppBufferMatrix[iBufferIndex]->GetGPUVirtualAddress());
}


