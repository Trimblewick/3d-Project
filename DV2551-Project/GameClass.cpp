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
	
	m_pGraphicsHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, m_iBackBufferCount);
	m_pCopyHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE_COPY, 5);


	//m_pGraphicsHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, m_iBackBufferCount, 2);

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

	m_pSwapChain = m_pD3DFactory->CreateSwapChain(&descSwapChain, m_pGraphicsHighway->GetCQ());//m_pGraphicsHighway->GetCQ());

	//create rtvs and descriptor heap
	m_pDHRTV = m_pD3DFactory->CreateDH(m_iBackBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
	int iSizeOffsetRTV = m_pD3DFactory->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = m_pDHRTV->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < m_iBackBufferCount; ++i)
	{
		DxAssert(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppRTV[i])));
		m_pD3DFactory->GetDevice()->CreateRenderTargetView(m_ppRTV[i], nullptr, handleDH);
		handleDH.ptr += iSizeOffsetRTV;
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
	descRasterizer.CullMode = D3D12_CULL_MODE_NONE;
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
	

	tempRS = m_pD3DFactory->CreateRS(&descRS);

	ID3DBlob* pVSblob = m_pD3DFactory->CompileShader(L"VertexShader2.hlsl", "vs_5_1");
	ID3DBlob* pPSblob = m_pD3DFactory->CompileShader(L"PixelShader.hlsl", "ps_5_1");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
	descPSO.pRootSignature = tempRS;
	descPSO.InputLayout = inputLayoutDesc;

	tempPSO = m_pD3DFactory->CreatePSO(&descPSO);

	SAFE_RELEASE(pVSblob);
	SAFE_RELEASE(pPSblob);

	m_pCamera = m_pD3DFactory->CreateCamera(m_iBackBufferCount, (long)pWindow->GetWidth(), (long)pWindow->GetHeight());

	//Create Bezier
	m_nrOfVertices = 3; //change to nrOfVertices which we get from plane class
	m_pBezierClass = m_pD3DFactory->CreateBezier(m_nrOfVertices);
	m_pBezierClass->CalculateBezierVertices(/*m_pPlaneClass->GetVertices()*/); //send vertices from plane and offset Y

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
	SAFE_RELEASE(tempPSO);
	SAFE_RELEASE(tempRS);

	if (m_pBezierClass)
	{
		delete m_pBezierClass;
		m_pBezierClass = nullptr;
	}

}

void GameClass::Update(Input * pInput, double dDeltaTime)
{
	int iFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	m_dDeltaTime = dDeltaTime;
	m_pCamera->Update(pInput, dDeltaTime, iFrameIndex);
	//m_pBezierClass->CalculateBezierVertices(); //calculates this frame's bézier vertices using previous frame's bézier vertices
	m_pBezierClass->UpdateBezierVertices(); //Calculates Bézier vertices
	ID3D12GraphicsCommandList* pCLtest = ClearBackBuffer();
	PresentBackBuffer(pCLtest);
}

ID3D12GraphicsCommandList* GameClass::ClearBackBuffer()
{
	int iFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	ID3D12GraphicsCommandList* pCL = m_pGraphicsHighway->GetFreshCL();

	D3D12_RESOURCE_TRANSITION_BARRIER transition = {};
	transition.pResource = m_ppRTV[iFrameIndex];
	transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	D3D12_RESOURCE_BARRIER barrierTransition = {};
	barrierTransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierTransition.Transition = transition;

	pCL->ResourceBarrier(1, &barrierTransition);
	int iIncrementSizeRTV = m_pD3DFactory->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = m_pDHRTV->GetCPUDescriptorHandleForHeapStart();
	handleDH.ptr += iIncrementSizeRTV * iFrameIndex;

	pCL->ClearRenderTargetView(handleDH, m_pClearColor, NULL, nullptr);
	pCL->OMSetRenderTargets(1, &handleDH, NULL, nullptr);
	return pCL;
}

bool waduheck = false;
void GameClass::PresentBackBuffer(ID3D12GraphicsCommandList* pCL)
{
	int iFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	pCL->SetGraphicsRootSignature(tempRS);

	m_pCamera->BindCamera(pCL, iFrameIndex);
	m_pBezierClass->BindBezier(pCL, iFrameIndex);
	pCL->SetPipelineState(tempPSO);
	if (!waduheck)
	{
		waduheck = true;
		m_pPlane = m_pD3DFactory->CreatePlane(pCL);
	
		pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCL->DrawInstanced(3, 1, 0, 0);
	}
	else
	{
		m_pPlane->bind(pCL);
	}
	D3D12_RESOURCE_TRANSITION_BARRIER transition = {};
	transition.pResource = m_ppRTV[iFrameIndex];
	transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; 	
	transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	D3D12_RESOURCE_BARRIER barrierTransition = {};
	barrierTransition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierTransition.Transition = transition;

	pCL->ResourceBarrier(1, &barrierTransition);
	pCL->Close();

	m_pGraphicsHighway->QueueCL(pCL);
	int test = m_pGraphicsHighway->ExecuteCQ();
	

	m_pSwapChain->Present(0, 0);
	m_pGraphicsHighway->Wait(test);
}

void GameClass::Frame()
{
	//for each rs
		//for each pipe
			//highway, get cls
			//for each object
}
