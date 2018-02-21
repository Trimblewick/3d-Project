#pragma once

#include "Input.h"
#include "D3DFactory.h"
#include "GPUHighway.h"
#include "Window.h"
#include "BezierClass.h"

class GameClass
{
private:

public:
	GameClass();
	~GameClass();

	bool							Initialize(Window* pWindow);
	void							CleanUp();

	void							Update(Input* input, double dDeltaTime);
	ID3D12GraphicsCommandList*							ClearBackBuffer();
	void							PrecentBackBuffer(ID3D12GraphicsCommandList* pCL);
	void							Frame();


	

private:
	static const unsigned int		m_iBackBufferCount = 3;
	double							m_dDeltaTime;
	float							m_pClearColor[4];


	D3DFactory*						m_pD3DFactory;
	//GPUHighway*						m_pGraphicsHighway;
	BezierClass*					m_pBezierClass;


	IDXGISwapChain3*				m_pSwapChain;
	ID3D12Resource*					m_ppRTV[m_iBackBufferCount];
	ID3D12DescriptorHeap*			m_pDHRTV;

	ID3D12RootSignature*			tempRS;
	ID3D12PipelineState*			tempPSO;
	ID3D12CommandQueue*				tempcq;

	ID3D12CommandAllocator*			tempcas[m_iBackBufferCount];
	ID3D12GraphicsCommandList*		tempcls[m_iBackBufferCount];
	ID3D12Fence*					tempFences[m_iBackBufferCount];
	size_t							tempFencevalues[m_iBackBufferCount];
	HANDLE							temphandle;


	D3D12_VIEWPORT						m_viewport;
	D3D12_RECT							m_rectScissor;
	
};
 
