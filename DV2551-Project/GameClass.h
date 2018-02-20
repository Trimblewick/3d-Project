#pragma once

#include "Input.h"
#include "D3DFactory.h"
#include "GPUHighway.h"
#include "Window.h"

class GameClass
{
private:

public:
	GameClass();
	~GameClass();

	bool							Initialize(Window* pWindow);
	void							CleanUp();

	void							Update(Input* input, double dDeltaTime);
	void							ClearBackBuffer();
	void							PrecentBackBuffer();
	void							Frame();


	

private:
	static const unsigned int		m_iBackBufferCount = 2;
	double							m_dDeltaTime;
	float							m_pClearColor[4];


	D3DFactory*						m_pD3DFactory;
	GPUHighway*						m_pGraphicsHighway;


	IDXGISwapChain3*				m_pSwapChain;
	ID3D12Resource*					m_ppRTV[m_iBackBufferCount];
	ID3D12DescriptorHeap*			m_pDHRTV;

	ID3D12RootSignature*			tempRS;
	ID3D12PipelineState*			tempPSO;
	
};
 
