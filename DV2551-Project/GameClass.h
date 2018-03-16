#pragma once

#include "Input.h"
#include "D3DFactory.h"
#include "GPUHighway.h"
#include "Window.h"
#include "BezierClass.h"
#include "Plane.h"
#include "D3D12Timer.hpp"

class GameClass
{
private:
	void							TransitionBackBufferIntoRenderTargetState();
	void							Frame();
	void							PresentBackBuffer();
public:
	GameClass();
	~GameClass();

	bool							Initialize(Window* pWindow);
	void							CleanUp();

	void							Update(Input* input, double dDeltaTime);
	

	

private:
	static const unsigned int		m_iBackBufferCount = 3;
	double							m_dDeltaTime;
	float							m_pClearColor[4];
	int								m_nrOfPatchesWidthX;
	int								m_nrOfPatchesWidthZ;

	D3DFactory*						m_pD3DFactory;
	GPUHighway*						m_pGraphicsHighway;
	GPUHighway*						m_pCopyHighway;
	Camera*							m_pCamera;
	int								m_iNrOfPlanes;
	BezierClass**					m_ppBezierClass;
	Plane*							m_pPlane;

	IDXGISwapChain3*				m_pSwapChain;
	ID3D12Resource*					m_ppRTV[m_iBackBufferCount];
	D3D12_RESOURCE_BARRIER			m_pBarrierTransitionToRTV[m_iBackBufferCount];
	D3D12_RESOURCE_BARRIER			m_pBarrierTransitionToPresent[m_iBackBufferCount];
	int								m_pRTVWaitIndex[m_iBackBufferCount];
	ID3D12DescriptorHeap*			m_pDHRTV;
	int								m_iIncrementSizeRTV;

	int								m_pCopyWaitIndex[m_iBackBufferCount];

	void							TicksToSeconds();

	ID3D12Resource*					m_pDSV;
	ID3D12DescriptorHeap*			m_pDHDSV;

	ID3D12RootSignature*			m_pRS;
	ID3D12PipelineState*			m_pPSO;

	D3D12Timer*						m_pGraphicsTimer;
	D3D12Timer*						m_pCopyTimer;

	unsigned long long				m_CPUcount;
	unsigned long long				m_CPUfrequency;

	struct TimingData {
		unsigned long long GPUCalibration;
		unsigned long long CPUCalibration;
		unsigned long long GPUFrequency;
		unsigned long long start;
		unsigned long long end;
	} m_pTimingData[2];
	
};
 
