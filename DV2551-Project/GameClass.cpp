#include "stdafx.h"

#include "GameClass.h"
#include "Window.h"

GameClass::GameClass()
{
	
}

GameClass::~GameClass()
{
	
}


bool GameClass::Initialize(Window* pWindow)
{
	m_pD3DFactory = new D3DFactory();
	
	m_pGraphicsHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, m_iBackBufferCount, 2);

	{	//set up swapchain with the graphics highway
		DXGI_MODE_DESC descMode = {};
		descMode.Width = pWindow->GetWidth();
		descMode.Height = pWindow->GetHeight();
		descMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_SAMPLE_DESC descSample = {};
		descSample.Count = 1;

		DXGI_SWAP_CHAIN_DESC descSwapChain = {};
		descSwapChain.BufferCount = m_iBackBufferCount;
		descSwapChain.BufferDesc = descMode;
		descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		descSwapChain.OutputWindow = pWindow->GetWindowHandle();
		descSwapChain.SampleDesc = descSample;
		descSwapChain.Windowed = true;

		m_pSwapChain = m_pD3DFactory->CreateSwapChain(&descSwapChain, m_pGraphicsHighway->GetCQ());
	}
	
	return true;
}

void GameClass::CleanUp()
{
	if (m_pD3DFactory)
	{
		delete m_pD3DFactory;
		m_pD3DFactory = nullptr;
	}

}

void GameClass::Update(Input * input, double dDeltaTime)
{
	m_dDeltaTime = dDeltaTime;
}

void GameClass::ClearBackBuffer()
{


}

void GameClass::PrecentBackBuffer()
{


}

void GameClass::Frame()
{

	//for each rs
		//for each pipe
			//highway, get cls
			//for each object
}
