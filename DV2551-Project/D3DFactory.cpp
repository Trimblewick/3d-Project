#include "stdafx.h"
#include "D3DFactory.h"
#include <wrl.h>

D3DFactory::D3DFactory()
{
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
	
	DxAssert(CreateDXGIFactory1(IID_PPV_ARGS(&m_pDXGIFactory)));

	IDXGIAdapter1* pTempAdapter = nullptr;
	IDXGIAdapter1* pAdapter = nullptr;
	int iAdapterID = 0;
	int iAdapterFound = 0;
	size_t iBestVideoMemory = 0;

	while (m_pDXGIFactory->EnumAdapters1(iAdapterID, &pTempAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc1Adapter;
		pTempAdapter->GetDesc1(&desc1Adapter);

		if (desc1Adapter.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			iAdapterID++;
			continue;
		}

		//create bogo-device to 
		if (SUCCEEDED(D3D12CreateDevice(pTempAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			iAdapterFound++;
			size_t iVideoMemory = desc1Adapter.DedicatedVideoMemory;

			if (iVideoMemory > iBestVideoMemory)//Pick the adapter with the largest vram
			{
				iBestVideoMemory = iVideoMemory;
				pAdapter = pTempAdapter;
			}
		}

		iAdapterID++;
	}

	assert(iAdapterFound > 0);

	DxAssert(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice)));
	
}

D3DFactory::~D3DFactory()
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDXGIFactory);
}

ID3D12Device * D3DFactory::GetDevice()
{
	return m_pDevice;
}

ID3D12CommandQueue * D3DFactory::CreateCQ(D3D12_COMMAND_LIST_TYPE type)
{
	ID3D12CommandQueue* pCQ;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type = type;

	DxAssert(m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&pCQ)));

	return pCQ;
}

ID3D12CommandAllocator * D3DFactory::CreateCA(D3D12_COMMAND_LIST_TYPE type)
{
	ID3D12CommandAllocator* pCA;

	DxAssert(m_pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&pCA)));

	return pCA;
}

ID3D12GraphicsCommandList * D3DFactory::CreateCL(ID3D12CommandAllocator * pCA, D3D12_COMMAND_LIST_TYPE type)
{
	ID3D12GraphicsCommandList* pCL;

	DxAssert(m_pDevice->CreateCommandList(0, type, pCA, nullptr, IID_PPV_ARGS(&pCL)));

	return pCL;
}

ID3D12Fence * D3DFactory::CreateFence()
{
	ID3D12Fence* pFence;

	DxAssert(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)));

	return pFence;
}

ID3D12RootSignature * D3DFactory::CreateRS(D3D12_ROOT_SIGNATURE_DESC* pDesc)
{
	ID3DBlob* pBlob;
	ID3D12RootSignature* pRS;

	DxAssert(D3D12SerializeRootSignature(pDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pBlob, nullptr));
	DxAssert(m_pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRS)));

	return pRS;
}

ID3D12PipelineState * D3DFactory::CreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC * pDesc)
{
	ID3D12PipelineState* pPSO;

	DxAssert(m_pDevice->CreateGraphicsPipelineState(pDesc, IID_PPV_ARGS(&pPSO)));

	return pPSO;
}

IDXGISwapChain3 * D3DFactory::CreateSwapChain(DXGI_SWAP_CHAIN_DESC* pDesc, ID3D12CommandQueue * pCQ)
{
	IDXGISwapChain*		pTemp = nullptr;
	IDXGISwapChain3*	pSwapChain = nullptr;

	DxAssert(m_pDXGIFactory->CreateSwapChain(pCQ, pDesc, &pTemp));

	pSwapChain = static_cast<IDXGISwapChain3*>(pTemp);

	return pSwapChain;
}

ID3D12DescriptorHeap * D3DFactory::CreateDH(int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, bool bShaderVisible)
{
	ID3D12DescriptorHeap* pDH;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	if (bShaderVisible)
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	DxAssert(m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDH)));

	return pDH;
}

ID3DBlob * D3DFactory::CompileShader(LPCWSTR filePath, LPCSTR shadermodel)
{
	ID3DBlob* shaderBlob;

	DxAssert(D3DCompileFromFile(filePath, nullptr, nullptr, "main", shadermodel,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &shaderBlob, nullptr));

	return shaderBlob;
}

ID3D12Resource * D3DFactory::CreateCommitedResource(D3D12_HEAP_TYPE type, D3D12_RESOURCE_DESC * pResourceDesc, D3D12_RESOURCE_STATES initState, D3D12_CLEAR_VALUE* clearValue)
{
	ID3D12Resource* pHeap;

	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = type;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.VisibleNodeMask = 1;
	heapProperties.CreationNodeMask = 1;

	DxAssert(m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		pResourceDesc,
		initState,
		clearValue,
		IID_PPV_ARGS(&pHeap)));

	return pHeap;
}

void D3DFactory::UpdateSubresources(ID3D12GraphicsCommandList * pCopyCL, ID3D12Resource * pBufferHeap, ID3D12Resource * pUploadHeap, D3D12_SUBRESOURCE_DATA * pBufferData)
{
	unsigned char** ppData = new unsigned char*;
	DxAssert(pUploadHeap->Map(0, NULL, reinterpret_cast<void**>(ppData)));

	size_t test = pBufferData->RowPitch;
	memcpy(*ppData, reinterpret_cast<const void*>(pBufferData->pData), pBufferData->RowPitch);
	pUploadHeap->Unmap(0, NULL);

	pCopyCL->CopyBufferRegion(pBufferHeap, 0, pUploadHeap, 0, pBufferHeap->GetDesc().Width);//CopyResource(pBufferHeap, pUploadHeap);
	delete[] ppData;
	//pCmdList->CopyBufferRegion(pVBuffer, 0, pVBUpload, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
}

GPUHighway * D3DFactory::CreateGPUHighway(D3D12_COMMAND_LIST_TYPE type, unsigned int iNumberOfCLs)
{
	ID3D12CommandQueue* pCQ = CreateCQ(type);

	ID3D12CommandAllocator** ppCAs = new ID3D12CommandAllocator*[iNumberOfCLs];
	ID3D12GraphicsCommandList** ppCLs = new ID3D12GraphicsCommandList*[iNumberOfCLs];
	ID3D12Fence** ppFences = new ID3D12Fence*[iNumberOfCLs];

	for (unsigned int i = 0; i < iNumberOfCLs; ++i)
	{
		ppCAs[i] = CreateCA(type);
		ppCLs[i] = CreateCL(ppCAs[i], type);
		DxAssert(ppCLs[i]->Close());
		ppFences[i] = CreateFence();
	}

	return new GPUHighway(type, pCQ, ppCAs, ppCLs, ppFences, iNumberOfCLs);
}

Camera * D3DFactory::CreateCamera(unsigned int iBufferCount, long iWidthWindow, long iHeightWindow)
{
	Camera::CameraBuffer data;
	data.forward.x = 0.0f;
	data.forward.y = 0.0f;
	data.forward.z = 1.0f;

	data.position.x = 0.0f;
	data.position.y = 0.0f;
	data.position.z = 0.0f;
	
	data.right.x = 1.0f;
	data.right.y = 0.0f;
	data.right.z = 0.0f;

	data.up.x = 0.0f;
	data.up.y = 1.0f;
	data.up.z = 0.0f;

	data.viewMat = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&data.position), DirectX::XMLoadFloat3(&data.forward), DirectX::XMLoadFloat3(&data.up)));
	data.projMat = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(45.0f*(DirectX::XM_PI / 180.0f), iWidthWindow / (float)iHeightWindow, 0.1f, 1000.0f));
	data.vpMat = DirectX::XMMatrixMultiply(data.projMat, data.viewMat);

	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (float)iWidthWindow;
	viewport.Height = (float)iHeightWindow;

	D3D12_RECT rectScissor;
	rectScissor.left = (long)0;
	rectScissor.top = (long)0;
	rectScissor.bottom = iHeightWindow;
	rectScissor.right = iWidthWindow;

	D3D12_RESOURCE_DESC descResource = {};
	descResource.Alignment = 0;
	descResource.DepthOrArraySize = 1;
	descResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descResource.Flags = D3D12_RESOURCE_FLAG_NONE;
	descResource.Format = DXGI_FORMAT_UNKNOWN;
	descResource.Height = 1;
	descResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descResource.MipLevels = 1;
	descResource.SampleDesc.Count = 1;
	descResource.SampleDesc.Quality = 0;
	descResource.Width = 65536;					//is there any reason not to have a 64kb width since it will be constant? alignment? Would it be faster to have a fixed size matching the buffer struct?
	
	ID3D12DescriptorHeap* pDH = CreateDH(3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	int iIncrementSizeCBV = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = pDH->GetCPUDescriptorHandleForHeapStart();

	unsigned char** ppBufferAddressPointer = new unsigned char*;
	ID3D12Resource** ppBufferMatrix = new ID3D12Resource*[iBufferCount];
	ID3D12Resource*	pUploadHeap = CreateCommitedResource(D3D12_HEAP_TYPE_UPLOAD, &descResource, D3D12_RESOURCE_STATE_GENERIC_READ);

	D3D12_CONSTANT_BUFFER_VIEW_DESC descCB = {};
	descCB.SizeInBytes = sizeof(data);

	
	for (unsigned int i = 0; i < iBufferCount; ++i)
	{
		ppBufferMatrix[i] = CreateCommitedResource(D3D12_HEAP_TYPE_DEFAULT, &descResource, D3D12_RESOURCE_STATE_COPY_DEST);

		descCB.BufferLocation = ppBufferMatrix[i]->GetGPUVirtualAddress();

		m_pDevice->CreateConstantBufferView(&descCB, handleDH);
		handleDH.ptr += iIncrementSizeCBV;
	}

	D3D12_RANGE rangeRead = {};
	DxAssert(pUploadHeap->Map(0, &rangeRead, reinterpret_cast<void**>(ppBufferAddressPointer)));
	
	return new Camera(data, viewport, rectScissor, pUploadHeap, ppBufferAddressPointer, ppBufferMatrix , iBufferCount, pDH);
}

BezierClass* D3DFactory::CreateBezier(int iWidthPlane)
{
	ID3D12DescriptorHeap* pDH = CreateDH(1, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	ID3D12Resource* pUploadCB = nullptr;
	ID3D12Resource* pCB = nullptr;
	int iNrOfPoints = 16;
	float4* pBezierPoints = new float4[iNrOfPoints];
	int iSize = sizeof(float4) * iNrOfPoints;
	int iWidth = iWidthPlane / 3;
	
	double* pPointsOffset = new double[iNrOfPoints];
	for (int i = 0; i < iNrOfPoints; ++i)
		pPointsOffset[i] = rand();

	int index = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			float4 temp;
			temp.x = iWidth * j;
			temp.y = sin(pPointsOffset[index]);
			temp.z = iWidth * i;
			temp.w = 1.0f;

			pBezierPoints[index] = temp;
			index++;
		}
	}

	//Set resource desc
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = iSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	pUploadCB = CreateCommitedResource(D3D12_HEAP_TYPE_UPLOAD, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ);
	pCB = CreateCommitedResource(D3D12_HEAP_TYPE_DEFAULT, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbDesc;
	m_cbDesc.BufferLocation = pUploadCB->GetGPUVirtualAddress();
	m_cbDesc.SizeInBytes = iSize;//already 256 bytes aligned

	m_pDevice->CreateConstantBufferView(&m_cbDesc, pDH->GetCPUDescriptorHandleForHeapStart());

	D3D12_RANGE range = { 0,0 }; //Entire range

	uint8_t* address;
	pUploadCB->Map(0, &range, reinterpret_cast<void**>(&address));
	memcpy(address, pBezierPoints, iSize);

	return new BezierClass(pDH, pUploadCB, pCB, address, iNrOfPoints, pBezierPoints, pPointsOffset);;
}

Plane * D3DFactory::CreatePlane(ID3D12GraphicsCommandList* pCmdList, unsigned int tiles, ID3D12Resource* pUploadHeapVertexBuffer, ID3D12Resource* pUploadHeapIndexBuffer)
{
	unsigned int uiWidth = tiles + 1;
	std::vector<float2> pVerts;
	pVerts.reserve(uiWidth * uiWidth);

	for (unsigned int i = 0; i < uiWidth; ++i)
	{
		for (unsigned int j = 0; j < uiWidth; ++j)
		{
			pVerts.push_back(float2{ (float)j / (float)tiles, (float)i / (float)tiles });
		}
	}
	float2* vList = pVerts.data();
	int vBufferSize = sizeof(float2) * pVerts.size();
	
	//Vertex Buffer ---------------------------------
	D3D12_RESOURCE_DESC bufferDesc;
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;
	bufferDesc.Width = (vBufferSize + 255) & ~255; //Bytes!
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pVBuffer = CreateCommitedResource(D3D12_HEAP_TYPE_DEFAULT, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST);
	pVBuffer->SetName(L"Vertex Buffer Heap");

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList);
	vertexData.RowPitch = vBufferSize;
	vertexData.SlicePitch = vBufferSize; //both are supposed to be size in bytes of all triangles...

	UpdateSubresources(pCmdList, pVBuffer, pUploadHeapVertexBuffer, &vertexData);
	
	D3D12_VERTEX_BUFFER_VIEW vbView;
	vbView.BufferLocation = pVBuffer->GetGPUVirtualAddress();
	vbView.StrideInBytes = sizeof(float2);
	vbView.SizeInBytes = vBufferSize;

	//Index buffer-----------------------

	unsigned int uiNumIndices = (uiWidth - 1) * (uiWidth - 1) * 6;
	std::vector<DWORD> pIndices;
	pIndices.reserve(uiNumIndices);

	for (unsigned int i = 0; i < (uiWidth - 1); ++i)
	{
		for (unsigned int j = 0; j < (uiWidth - 1); ++j)
		{
			pIndices.push_back(i * uiWidth + j + uiWidth);
			pIndices.push_back(i * uiWidth + j);
			pIndices.push_back(i * uiWidth + j + uiWidth + 1);
			pIndices.push_back(i * uiWidth + j + uiWidth + 1);
			pIndices.push_back(i * uiWidth + j);
			pIndices.push_back(i * uiWidth + j + 1);
		}
	}

	DWORD *iList = pIndices.data();
	int iBufferSize = sizeof(DWORD) * pIndices.size();

	D3D12_RESOURCE_DESC indexBufferDesc;
	indexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	indexBufferDesc.Alignment = 0;
	indexBufferDesc.Width = (iBufferSize + 255) & ~255; //Bytes!
	indexBufferDesc.Height = 1;
	indexBufferDesc.DepthOrArraySize = 1;
	indexBufferDesc.MipLevels = 1;
	indexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	indexBufferDesc.SampleDesc.Count = 1;
	indexBufferDesc.SampleDesc.Quality = 0;
	indexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	indexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pIBuffer = CreateCommitedResource(D3D12_HEAP_TYPE_DEFAULT, &indexBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST);
	pIBuffer->SetName(L"IBuffer Resource Heap");

	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(iList); // pointer to our index array
	indexData.RowPitch = iBufferSize; // size of all our index buffer
	indexData.SlicePitch = iBufferSize; // also the size of our index buffer

	UpdateSubresources(pCmdList, pIBuffer, pUploadHeapIndexBuffer, &indexData);

	D3D12_INDEX_BUFFER_VIEW ibView;
	ibView.BufferLocation = pIBuffer->GetGPUVirtualAddress();
	ibView.SizeInBytes = iBufferSize;
	ibView.Format = DXGI_FORMAT_R32_UINT;

	return new Plane(tiles, pVBuffer, vbView, pIBuffer, ibView);
}
