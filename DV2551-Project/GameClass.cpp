#include "stdafx.h"

#include "GameClass.h"
#include <d3d12.h>


GameClass::GameClass()
{
	
}

GameClass::~GameClass()
{
	
}


bool GameClass::Initialize(Window* pWindow)
{
	m_pD3DFactory = new D3DFactory();
	
	m_pGraphicsHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, 15);
	m_pCopyHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE_COPY, 5);


	//set up swapchain with the graphics highway
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

	//create rtvs and descriptor heap
	m_pDHRTV = m_pD3DFactory->CreateDH(m_iBackBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
	m_iIncrementSizeRTV = m_pD3DFactory->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = m_pDHRTV->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < m_iBackBufferCount; ++i)
	{
		DxAssert(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppRTV[i])));
		m_pD3DFactory->GetDevice()->CreateRenderTargetView(m_ppRTV[i], nullptr, handleDH);
		handleDH.ptr += m_iIncrementSizeRTV;
		m_pRTVWaitIndex[i] = 0;
	}
	m_pClearColor[0] = 0.1f;
	m_pClearColor[1] = 0.5f;
	m_pClearColor[2] = 0.3f;
	m_pClearColor[3] = 1.0f;
	
	
	//set up default blend
	D3D12_RENDER_TARGET_BLEND_DESC descBlendStateRTV = {};
	descBlendStateRTV.BlendEnable = false;
	descBlendStateRTV.BlendOp = D3D12_BLEND_OP_ADD;
	descBlendStateRTV.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	descBlendStateRTV.DestBlend = D3D12_BLEND_ZERO;
	descBlendStateRTV.DestBlendAlpha = D3D12_BLEND_ZERO;
	descBlendStateRTV.LogicOp = D3D12_LOGIC_OP_NOOP;
	descBlendStateRTV.LogicOpEnable = false;
	descBlendStateRTV.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	descBlendStateRTV.SrcBlend = D3D12_BLEND_ONE;
	descBlendStateRTV.SrcBlendAlpha = D3D12_BLEND_ONE;
	
	D3D12_BLEND_DESC descBlendSate = {};
	descBlendSate.AlphaToCoverageEnable = false;
	descBlendSate.IndependentBlendEnable = false;
	for (int i = 0; i < m_iBackBufferCount; ++i)
		descBlendSate.RenderTarget[i] = descBlendStateRTV;

	D3D12_RASTERIZER_DESC descRasterizer = {};
	descRasterizer.FillMode = D3D12_FILL_MODE_SOLID;
	descRasterizer.CullMode = D3D12_CULL_MODE_BACK;
	descRasterizer.FrontCounterClockwise = false;
	descRasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	descRasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	descRasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descRasterizer.DepthClipEnable = true;
	descRasterizer.MultisampleEnable = false;
	descRasterizer.AntialiasedLineEnable = false;
	descRasterizer.ForcedSampleCount = 0;
	descRasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//Constant buffer RootSig setup
	D3D12_ROOT_DESCRIPTOR d = {};

	D3D12_ROOT_DESCRIPTOR cbvDescriptor;
	cbvDescriptor.RegisterSpace = 0;
	cbvDescriptor.ShaderRegister = 1;


	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].Descriptor = d;
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor = cbvDescriptor;


	D3D12_ROOT_SIGNATURE_DESC descRS = {};
	descRS.NumParameters = 2;
	descRS.pParameters = rootParameters;
	descRS.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	

	m_pRS = m_pD3DFactory->CreateRS(&descRS);

	ID3DBlob* pVSblob = m_pD3DFactory->CompileShader(L"VertexShader2.hlsl", "vs_5_1");
	ID3DBlob* pPSblob = m_pD3DFactory->CompileShader(L"PixelShader.hlsl", "ps_5_1");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC descPSO = {};
	descPSO.BlendState = descBlendSate;
	//descPSO.DepthStencilState
	descPSO.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	descPSO.NumRenderTargets = 1;
	descPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPSO.VS = { pVSblob->GetBufferPointer(), pVSblob->GetBufferSize() };
	descPSO.PS = { pPSblob->GetBufferPointer(), pPSblob->GetBufferSize() };
	descPSO.RasterizerState = descRasterizer;
	descPSO.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	descPSO.SampleDesc = descSample;
	descPSO.SampleMask = 0xffffffff;
	descPSO.pRootSignature = m_pRS;
	descPSO.InputLayout = inputLayoutDesc;

	m_pPSO = m_pD3DFactory->CreatePSO(&descPSO);

	SAFE_RELEASE(pVSblob);
	SAFE_RELEASE(pPSblob);

	m_pCamera = m_pD3DFactory->CreateCamera(m_iBackBufferCount, (long)pWindow->GetWidth(), (long)pWindow->GetHeight());

	ID3D12GraphicsCommandList* pCL = m_pCopyHighway->GetFreshCL();
	m_pPlane = m_pD3DFactory->CreatePlane(pCL, 16);
	m_pCopyHighway->QueueCL(pCL);
	m_pCopyHighway->Wait(m_pCopyHighway->ExecuteCQ());


	//Create Bezier
	int planeWidth = m_pPlane->GetWidth();
	m_nrOfVertices = 16;
	m_pBezierClass = m_pD3DFactory->CreateBezier(m_nrOfVertices);
	m_pBezierClass->CalculateBezierPoints(planeWidth/*, 2*/);
	m_nrOfPatches = 1; //must be 1 or factor of 2

	return true;
}

void GameClass::CleanUp()
{
	if (m_pD3DFactory)
	{
		delete m_pD3DFactory;
		m_pD3DFactory = nullptr;
	}
	if (m_pGraphicsHighway)
	{
		delete m_pGraphicsHighway;
		m_pGraphicsHighway = nullptr;
	}
	if (m_pCopyHighway)
	{
		delete m_pCopyHighway;
		m_pCopyHighway = nullptr;
	}
	if (m_pPlane)
	{
		delete m_pPlane;
		m_pPlane = nullptr;
	}
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	SAFE_RELEASE(m_pSwapChain);
	
	for (int i = 0; i < m_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_ppRTV[i]);
	}
	SAFE_RELEASE(m_pDHRTV);
	SAFE_RELEASE(m_pPSO);
	SAFE_RELEASE(m_pRS);





	if (m_pBezierClass)
	{
		delete m_pBezierClass;
		m_pBezierClass = nullptr;
	}

}

int test = 0;
void GameClass::Update(Input * pInput, double dDeltaTime)
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	
	m_pGraphicsHighway->Wait(m_pRTVWaitIndex[iBufferIndex]);
	
	m_dDeltaTime = dDeltaTime;

	//for (int x = 0; x < m_nrOfPatches; ++x)
	//{
	//	for (int y = 0; m_nrOfPatches; ++y)
	//	{
	//		
	//		/*freshCL;
	//		//update bPoints(x, y)
	//		queueCL
	//		executeCL*/
	//	}
	//	
	//}

	ID3D12GraphicsCommandList* pCopyCL = m_pCopyHighway->GetFreshCL();

	m_pCamera->Update(pInput, dDeltaTime, iBufferIndex, pCopyCL);
	m_pBezierClass->UpdateBezierPoints(); //Calculates Bézier vertices
	//m_pBezierClass->BindBezier(pCopyCL, iBufferIndex); ???

	
	m_pCopyHighway->QueueCL(pCopyCL);
	int iCameraFence = m_pCopyHighway->ExecuteCQ();
	
	TransitionBackBufferIntoRenderTargetState();
	m_pCopyHighway->Wait(iCameraFence);

	Frame();
	PresentBackBuffer();
}

void GameClass::TransitionBackBufferIntoRenderTargetState()
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	
	ID3D12GraphicsCommandList* pCL = m_pGraphicsHighway->GetFreshCL();

	D3D12_RESOURCE_TRANSITION_BARRIER transition = {};
	transition.pResource = m_ppRTV[iBufferIndex];
	transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	D3D12_RESOURCE_BARRIER barrierTransition = {};
	barrierTransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierTransition.Transition = transition;

	pCL->ResourceBarrier(1, &barrierTransition);
	
	m_pGraphicsHighway->QueueCL(pCL);
}

void GameClass::Frame()
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = m_pDHRTV->GetCPUDescriptorHandleForHeapStart();
	handleDH.ptr += m_iIncrementSizeRTV * iBufferIndex;

	ID3D12GraphicsCommandList* pCL = m_pGraphicsHighway->GetFreshCL(m_pPSO);

	pCL->ClearRenderTargetView(handleDH, m_pClearColor, NULL, nullptr);
	pCL->OMSetRenderTargets(1, &handleDH, NULL, nullptr);

	pCL->SetGraphicsRootSignature(m_pRS);

	m_pCamera->BindCamera(pCL, iBufferIndex);
	m_pBezierClass->BindBezier(pCL, iBufferIndex);

	//m_pBezierClass->BindBezier(pCL, iBufferIndex);
	
	m_pPlane->bind(pCL);
	
	
	m_pGraphicsHighway->QueueCL(pCL);

	//update<-cpu
	//copy<-Q->fence1

	//update
	//copy<-Q->fence2

	//wait fence1
	//transition<-graphicsQ
	//draw
	//transition --

	//update
	//copy<-Q->fence3

	//wait fence2
	//transition<-graphicsQ
	//draw
	//transition --
}


void GameClass::PresentBackBuffer()
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	ID3D12GraphicsCommandList* pCL = m_pGraphicsHighway->GetFreshCL();

	
	D3D12_RESOURCE_TRANSITION_BARRIER transition = {};
	transition.pResource = m_ppRTV[iBufferIndex];
	transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	D3D12_RESOURCE_BARRIER barrierTransition = {};
	barrierTransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierTransition.Transition = transition;

	m_pCamera->UnbindCamera(pCL, iBufferIndex);
	pCL->ResourceBarrier(1, &barrierTransition);

	m_pGraphicsHighway->QueueCL(pCL);

	m_pRTVWaitIndex[iBufferIndex] = m_pGraphicsHighway->ExecuteCQ();

	m_pSwapChain->Present(0, 0);
}
