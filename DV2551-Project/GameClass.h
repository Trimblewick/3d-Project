#pragma once

#include "Input.h"
#include "D3DFactory.h"
#include "GPUHighway.h"

class GameClass
{
private:

public:
	GameClass();
	~GameClass();

	bool Initialize();
	void CleanUp();

	void Update(Input* input, double dDeltaTime);
	void ClearBackBuffer();
	void PrecentBackBuffer();
	void Frame();


	

private:
	const unsigned int		m_iBackBufferCount = 2;
	double					m_dDeltaTime;

	D3DFactory*				m_pD3DFactory;
	GPUHighway*				m_pGPUHighway;

	IDXGISwapChain3*		m_pSwapChain;
	
};
 
