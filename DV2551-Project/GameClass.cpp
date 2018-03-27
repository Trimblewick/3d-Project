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
	bool bDirectOnly = false;
	srand(time(NULL));
	m_pD3DFactory = new D3DFactory();
	
	m_pGraphicsHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, 15);
	if (bDirectOnly)
		m_pCopyHighway = m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE_DIRECT, 6);
	else
		m_pCopyHighway = m_pGraphicsHighway;//m_pD3DFactory->CreateGPUHighway(D3D12_COMMAND_LIST_TYPE_COPY, 6);

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

	D3D12_RESOURCE_TRANSITION_BARRIER transitionTo = {};
	transitionTo.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	transitionTo.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	D3D12_RESOURCE_TRANSITION_BARRIER transitionFrom = {};
	transitionFrom.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	transitionFrom.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	for (int i = 0; i < m_iBackBufferCount; ++i)
	{
		DxAssert(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppRTV[i])));
		m_pD3DFactory->GetDevice()->CreateRenderTargetView(m_ppRTV[i], nullptr, handleDH);
		handleDH.ptr += m_iIncrementSizeRTV;
		m_pRTVWaitIndex[i] = -1;
		m_pCopyWaitIndex[i] = -1;

		transitionTo.pResource = m_ppRTV[i];
		transitionFrom.pResource = m_ppRTV[i];

		m_pBarrierTransitionToRTV[i] = {};
		m_pBarrierTransitionToRTV[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		m_pBarrierTransitionToRTV[i].Transition = transitionTo;

		m_pBarrierTransitionToPresent[i] = {};
		m_pBarrierTransitionToPresent[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		m_pBarrierTransitionToPresent[i].Transition = transitionFrom;
	}
	m_pClearColor[0] = 0.5f;
	m_pClearColor[1] = 0.5f;
	m_pClearColor[2] = 0.8f;
	m_pClearColor[3] = 1.0f;
	
	m_pDHDSV = m_pD3DFactory->CreateDH(1, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false);

	D3D12_CLEAR_VALUE clearValueDSV = {};
	clearValueDSV.DepthStencil.Depth = 1.0f;
	clearValueDSV.DepthStencil.Stencil = 0;
	clearValueDSV.Format = DXGI_FORMAT_D32_FLOAT;

	D3D12_RESOURCE_DESC resourceDescDSV;
	resourceDescDSV.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescDSV.Alignment = 0;
	resourceDescDSV.Width = pWindow->GetWidth();
	resourceDescDSV.Height = pWindow->GetHeight();
	resourceDescDSV.DepthOrArraySize = 1;
	resourceDescDSV.MipLevels = 1;
	resourceDescDSV.Format = DXGI_FORMAT_D32_FLOAT;
	resourceDescDSV.SampleDesc.Count = 1;
	resourceDescDSV.SampleDesc.Quality = 0;
	resourceDescDSV.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDescDSV.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	m_pDSV = m_pD3DFactory->CreateCommitedResource(D3D12_HEAP_TYPE_DEFAULT, &resourceDescDSV, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValueDSV);
	
	D3D12_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = D3D12_DSV_FLAG_NONE;

	m_pD3DFactory->GetDevice()->CreateDepthStencilView(m_pDSV, &descDSV, m_pDHDSV->GetCPUDescriptorHandleForHeapStart());
	
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
	D3D12_ROOT_DESCRIPTOR cameraDescriptor = {};

	D3D12_ROOT_DESCRIPTOR bezierDescriptor;
	bezierDescriptor.RegisterSpace = 0;
	bezierDescriptor.ShaderRegister = 1;

	D3D12_ROOT_CONSTANTS bezierOffsetRootConstant = {};
	bezierOffsetRootConstant.Num32BitValues = 2;
	bezierOffsetRootConstant.ShaderRegister = 2;
	bezierOffsetRootConstant.RegisterSpace = 0;


	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].Descriptor = cameraDescriptor;
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor = bezierDescriptor;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[2].Constants = bezierOffsetRootConstant;

	D3D12_ROOT_SIGNATURE_DESC descRS = {};
	descRS.NumParameters = _countof(rootParameters);
	descRS.pParameters = rootParameters;
	descRS.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	

	m_pRS = m_pD3DFactory->CreateRS(&descRS);

	ID3DBlob* pVSblob = m_pD3DFactory->CompileShader(L"VertexShader2.hlsl", "vs_5_1");
	ID3DBlob* pGSblob = m_pD3DFactory->CompileShader(L"GeometryShader.hlsl", "gs_5_1");
	ID3DBlob* pPSblob = m_pD3DFactory->CompileShader(L"PixelShader.hlsl", "ps_5_1");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		//{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	D3D12_DEPTH_STENCILOP_DESC descDepthStencilOp;
	descDepthStencilOp.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	descDepthStencilOp.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	descDepthStencilOp.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	descDepthStencilOp.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_DEPTH_STENCIL_DESC descDepthStencil = {};
	descDepthStencil.DepthEnable = true;
	descDepthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	descDepthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.StencilEnable = false;
	descDepthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	descDepthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	descDepthStencil.BackFace = descDepthStencilOp;
	descDepthStencil.FrontFace = descDepthStencilOp;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC descPSO = {};
	descPSO.BlendState = descBlendSate;
	descPSO.DepthStencilState = descDepthStencil;
	descPSO.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	descPSO.NumRenderTargets = 1;
	descPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPSO.VS = { pVSblob->GetBufferPointer(), pVSblob->GetBufferSize() };
	descPSO.GS = { pGSblob->GetBufferPointer(), pGSblob->GetBufferSize() };
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

	D3D12_RESOURCE_DESC descHeap;
	descHeap.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descHeap.Alignment = 0;
	descHeap.Width = 65536;
	descHeap.Height = 1;
	descHeap.DepthOrArraySize = 1;
	descHeap.MipLevels = 1;
	descHeap.Format = DXGI_FORMAT_UNKNOWN;
	descHeap.SampleDesc.Count = 1;
	descHeap.SampleDesc.Quality = 0;
	descHeap.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descHeap.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pUploadHeapVertexBuffer = m_pD3DFactory->CreateCommitedResource(D3D12_HEAP_TYPE_UPLOAD, &descHeap, D3D12_RESOURCE_STATE_GENERIC_READ);
	ID3D12Resource* pUploadHeapIndexBuffer = m_pD3DFactory->CreateCommitedResource(D3D12_HEAP_TYPE_UPLOAD, &descHeap, D3D12_RESOURCE_STATE_GENERIC_READ);

	ID3D12GraphicsCommandList* pCL = m_pCopyHighway->GetFreshCL();
	m_pPlane = m_pD3DFactory->CreatePlane(pCL, 16, pUploadHeapVertexBuffer, pUploadHeapIndexBuffer);
	m_pCopyHighway->QueueCL(pCL);
	m_pCopyHighway->Wait(m_pCopyHighway->ExecuteCQ());
	SAFE_RELEASE(pUploadHeapVertexBuffer);//Only for init...
	SAFE_RELEASE(pUploadHeapIndexBuffer);

	m_iNrOfPlanes = 1024;
	m_ppBezierClass = new BezierClass*[m_iNrOfPlanes];
	for (int i = 0; i < m_iNrOfPlanes; ++i)
	{
		m_ppBezierClass[i] = m_pD3DFactory->CreateBezier(m_pPlane->GetWidth(), m_iBackBufferCount, i);
	}


	//BezierClass::SetUploadHeaps(nullptr);

	//LARGE_INTEGER tempFreq;
	//QueryPerformanceCounter(&tempFreq);
	//m_CPUcount = tempFreq.QuadPart;
	//QueryPerformanceFrequency(&tempFreq);
	//m_CPUfrequency = tempFreq.QuadPart;

	//m_pGraphicsTimer = new D3D12Timer(m_pD3DFactory->GetDevice(), D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_TIMESTAMP);
	//if (bDirectOnly)
	//	m_pCopyTimer = new D3D12Timer(m_pD3DFactory->GetDevice(), D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_TIMESTAMP);
	//else
	//	m_pCopyTimer = new D3D12Timer(m_pD3DFactory->GetDevice(), D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_COPY_QUEUE_TIMESTAMP);

	//m_pCopyHighway->GetCQ()->GetClockCalibration(&m_pTimingData[0].GPUCalibration, &m_pTimingData[0].CPUCalibration);
	//m_pGraphicsHighway->GetCQ()->GetClockCalibration(&m_pTimingData[1].GPUCalibration, &m_pTimingData[1].CPUCalibration);

	//m_pCopyHighway->GetCQ()->GetTimestampFrequency(&m_pTimingData[0].GPUFrequency);
	//m_pGraphicsHighway->GetCQ()->GetTimestampFrequency(&m_pTimingData[1].GPUFrequency);

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
	
	for (int i = 0; i < m_iNrOfPlanes; ++i)
	{
		delete m_ppBezierClass[i];
	}
	delete[] m_ppBezierClass;

	SAFE_RELEASE(m_pDSV);
	SAFE_RELEASE(m_pDHDSV);
	for (int i = 0; i < m_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_ppRTV[i]);
	}
	SAFE_RELEASE(m_pDHRTV);
	SAFE_RELEASE(m_pPSO);
	SAFE_RELEASE(m_pRS);
	SAFE_RELEASE(m_pSwapChain);
	//if (m_pGraphicsTimer)
	//{
	//	delete m_pGraphicsTimer;
	//	m_pGraphicsTimer = nullptr;
	//}
	//if (m_pCopyTimer)
	//{
	//	delete m_pCopyTimer;
	//	m_pCopyTimer = nullptr;
	//}


}

void GameClass::Update(Input * pInput, double dDeltaTime)
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	m_dDeltaTime = dDeltaTime;

	ID3D12GraphicsCommandList* pCopyCL = m_pCopyHighway->GetFreshCL();
	if (pCopyCL == nullptr)
		int stop = 0;
	//m_pCopyTimer->Start(pCopyCL);
	int i = 0;
	for (; i < m_iNrOfPlanes; ++i)
	{
		m_ppBezierClass[i]->UpdateBezierPoints(pCopyCL, m_dDeltaTime, iBufferIndex);
	}

	//m_pCopyTimer->Stop(pCopyCL);
	//m_pCopyTimer->ResolveQuery(pCopyCL);
	//m_pCopyHighway->Wait(m_pCopyWaitIndex[iBufferIndex]);	//WAIT COPY

	m_pGraphicsHighway->Wait(m_pRTVWaitIndex[iBufferIndex]);
	m_pCopyHighway->QueueCL(pCopyCL);
	//m_pCopyWaitIndex[iBufferIndex] = m_pCopyHighway->ExecuteCQ();

	m_dDeltaTime = dDeltaTime;
	m_pCamera->Update(pInput, dDeltaTime, iBufferIndex);
	
	Frame();
}

void GameClass::TransitionBackBufferIntoRenderTargetState()
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	
	/*ID3D12GraphicsCommandList* pCL = m_pGraphicsHighway->GetFreshCL();

	m_pGraphicsHighway->QueueCL(pCL);*/
}

void GameClass::Frame()
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = m_pDHRTV->GetCPUDescriptorHandleForHeapStart();
	handleDH.ptr += m_iIncrementSizeRTV * iBufferIndex;

	ID3D12GraphicsCommandList* pGraphicsCL = m_pGraphicsHighway->GetFreshCL(m_pPSO);
	
	//m_pGraphicsTimer->Start(pGraphicsCL);

	pGraphicsCL->ResourceBarrier(1, &m_pBarrierTransitionToRTV[iBufferIndex]);
	pGraphicsCL->ClearRenderTargetView(handleDH, m_pClearColor, NULL, nullptr);
	pGraphicsCL->ClearDepthStencilView(m_pDHDSV->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	
	pGraphicsCL->OMSetRenderTargets(1, &handleDH, NULL, &m_pDHDSV->GetCPUDescriptorHandleForHeapStart());
	pGraphicsCL->SetGraphicsRootSignature(m_pRS);
	m_pCamera->BindCamera(pGraphicsCL, iBufferIndex);
	m_pPlane->bind(pGraphicsCL);

	for (int i = 0; i < m_iNrOfPlanes; ++i)
	{
		m_ppBezierClass[i]->BindBezier(pGraphicsCL, iBufferIndex);							//Queue uploaded graphics
		pGraphicsCL->SetGraphicsRoot32BitConstant(2, (m_pPlane->GetWidth() - 4) * (i % (int)std::sqrt(m_iNrOfPlanes)), 0);
		pGraphicsCL->SetGraphicsRoot32BitConstant(2, (m_pPlane->GetWidth() - 4) * (i / (int)std::sqrt(m_iNrOfPlanes)), 1);
		m_pPlane->draw(pGraphicsCL);
		m_ppBezierClass[i]->UnbindBezier(pGraphicsCL, iBufferIndex);

	}

	m_pCamera->UnbindCamera(pGraphicsCL, iBufferIndex);
	pGraphicsCL->ResourceBarrier(1, &m_pBarrierTransitionToPresent[iBufferIndex]);
	//m_pGraphicsTimer->Stop(pGraphicsCL);
	//m_pGraphicsTimer->ResolveQuery(pGraphicsCL);

	

	//if (iBufferIndex == 0)
	//{
	//	m_pCopyTimer->CalculateTime();
	//	m_pTimingData[0].start = m_pCopyTimer->GetBeginTime();
	//	m_pCopyTimer->GetDeltaTime();
	//	m_pTimingData[0].end = m_pCopyTimer->GetEndTime();

	//	TicksToSeconds();
	//}
	//{
	//	m_pGraphicsTimer->CalculateTime();
	//	m_pTimingData[1].start = m_pGraphicsTimer->GetBeginTime();
	//	m_pGraphicsTimer->GetDeltaTime();
	//	m_pTimingData[1].end = m_pGraphicsTimer->GetEndTime();
	//}

	m_pGraphicsHighway->QueueCL(pGraphicsCL);
	m_pRTVWaitIndex[iBufferIndex] = m_pGraphicsHighway->ExecuteCQ();

	m_pSwapChain->Present(0, 0);
}


void GameClass::PresentBackBuffer()
{
	int iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	//ID3D12GraphicsCommandList* pCL = m_pGraphicsHighway->GetFreshCL();

	
}

void GameClass::TicksToSeconds()
{
	//Convert ticks to second for CPU/GPU calibration, start and end
	double CPUCalibration[2] = { 0. };
	double GPUCalibration[2] = { 0. };
	double start[2] = { 0. };
	double end[2] = { 0. };
	for (int i = 0; i < 2; ++i)
	{
		CPUCalibration[i] = (double)m_pTimingData[i].CPUCalibration / (double)m_CPUfrequency; //converts ticks per second to seconds
		GPUCalibration[i] = (double)m_pTimingData[i].GPUCalibration / (double)m_pTimingData[i].GPUFrequency;
		start[i] = (m_pTimingData[i].start - m_pTimingData[i].GPUCalibration) / (double)1000000000;
		end[i] = (m_pTimingData[i].end - m_pTimingData[i].GPUCalibration) / (double)1000000000;
	}

	

	//Calculate offset for CQ1
	double offset = CPUCalibration[0] - CPUCalibration[1] - GPUCalibration[0] + GPUCalibration[1];

	//Offset start and end for CQ1
	start[1] -= offset;
	end[1] -= offset;

	double start0 = start[0];
	double start1 = start[1];

	double end0 = end[0];
	double end1 = end[1];
	if (start[0] < end[1] && start[1] < end[0])
	{
		int stopper = 0;
	}
	
}