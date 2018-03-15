#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include "Input.h"

class Camera
{
public:
	struct CameraBuffer
	{
		DirectX::XMMATRIX vpMat;
		DirectX::XMMATRIX viewMat;
		DirectX::XMMATRIX projMat;
		DirectX::XMFLOAT3   position;
		float pad;
		DirectX::XMFLOAT3	forward;
		float pad2;
		DirectX::XMFLOAT3	right;
		float pad3;
		DirectX::XMFLOAT3	up;
		float pad4;
	};

	Camera(CameraBuffer data, D3D12_VIEWPORT viewport, D3D12_RECT scissorRect, ID3D12Resource* pUploadHeap, unsigned char** ppUploadHeapAdressPointer,
		ID3D12Resource** ppBufferMatrixHeap, unsigned int iBufferCount, ID3D12DescriptorHeap* pDH);
	~Camera();

	void									Update(Input* pInput, double dDeltaTime, unsigned int iBufferIndex);
	void									TransitionToConstant(ID3D12GraphicsCommandList* pCL, unsigned int iBufferIndex);
	void									TransitionToCopyDest(ID3D12GraphicsCommandList* pCL, unsigned int iBufferIndex);
	void									BindCamera(ID3D12GraphicsCommandList* pCL, unsigned int iBufferIndex);
	void									UnbindCamera(ID3D12GraphicsCommandList * pCL, unsigned int iBufferIndex);
private:
	D3D12_VIEWPORT							m_viewport;
	D3D12_RECT								m_rectscissor;

	D3D12_RESOURCE_BARRIER*					m_pTransitionToConstant;
	D3D12_RESOURCE_BARRIER*					m_pTransitionToCopyDest;

	CameraBuffer							m_data;

	unsigned int							m_iBufferCount;
	ID3D12Resource**						m_ppBufferMatrixHeap;
	ID3D12Resource*							m_pUploadHeap;
	unsigned char**							m_ppUploadHeapAdressPointer;
	ID3D12DescriptorHeap*					m_pDH;
};
