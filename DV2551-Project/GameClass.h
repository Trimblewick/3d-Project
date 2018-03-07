#pragma once

#include "Input.h"
#include "D3DFactory.h"
#include "GPUHighway.h"
#include "Window.h"
#include "BezierClass.h"
#include "Plane.h"

class GameClass
{
private:

public:
	GameClass();
	~GameClass();

	bool							Initialize(Window* pWindow);
	void							CleanUp();

	void							Update(Input* input, double dDeltaTime);
	void							TransitionBackBufferIntoRenderTargetState();
	void							PrecentBackBuffer();
	void							Frame();

	

private:
	static const unsigned int		m_iBackBufferCount = 3;
	double							m_dDeltaTime;
	float							m_pClearColor[4];
	int								m_nrOfVertices;

	D3DFactory*						m_pD3DFactory;
	GPUHighway*						m_pGraphicsHighway;
	GPUHighway*						m_pCopyHighway;
	Camera*							m_pCamera;
	BezierClass*					m_pBezierClass;
	Plane*							m_pPlane;

	IDXGISwapChain3*				m_pSwapChain;
	ID3D12Resource*					m_ppRTV[m_iBackBufferCount];
	int								m_pRTVWaitIndex[m_iBackBufferCount];
	ID3D12DescriptorHeap*			m_pDHRTV;
	int								m_iIncrementSizeRTV;

	ID3D12RootSignature*			m_pRS;
	ID3D12PipelineState*			m_pPSO;

	D3D12_VIEWPORT					m_viewport;
	D3D12_RECT						m_rectScissor;

};
 
