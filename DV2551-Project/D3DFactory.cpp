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

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

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
	descResource.Width = 65536;//sizeof(Camera::CameraBuffer);

	ID3D12DescriptorHeap* pDH = CreateDH(3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	int iIncrementSizeCBV = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE handleDH = pDH->GetCPUDescriptorHandleForHeapStart();
	unsigned char** ppBufferAdressPointers = new unsigned char*[iBufferCount];
	ID3D12Resource** ppBufferMatrix = new ID3D12Resource*[iBufferCount];
	D3D12_CONSTANT_BUFFER_VIEW_DESC descCB = {};
	descCB.SizeInBytes = sizeof(data);
	for (unsigned int i = 0; i < iBufferCount; ++i)
	{
		m_pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &descResource, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&ppBufferMatrix[i]));
	
		D3D12_RANGE rangeRead = {};

		descCB.BufferLocation = ppBufferMatrix[i]->GetGPUVirtualAddress();
		m_pDevice->CreateConstantBufferView(&descCB, handleDH);
		handleDH.ptr += iIncrementSizeCBV;

		HRESULT  hr = ppBufferMatrix[i]->Map(0, &rangeRead, reinterpret_cast<void**>(&ppBufferAdressPointers[i]));
		
		memcpy(ppBufferAdressPointers[i], &data, sizeof(data));
	}
	

	return new Camera(data, viewport, rectScissor, ppBufferMatrix, ppBufferAdressPointers, iBufferCount, pDH);
}

Plane * D3DFactory::CreatePlane(ID3D12GraphicsCommandList* pCmdList)
{
	int size = 16;
	Plane* plane = new Plane(size);

	/*struct Vertex
	{
		float4 position;
		float4 color;
	};
	Vertex vList[] = {
		{ -0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f }
	};*/
	float4 *vList = plane->GetVertices().data();
	int vBufferSize = sizeof(float) * 4 /*float4*/ * size * size; //sizeof(vList);

	//Vertex Buffer ---------------------------------

	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.VisibleNodeMask = 1;
	heapProperties.CreationNodeMask = 1;

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

	ID3D12Resource* pVBuffer;
	this->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&pVBuffer));

	pVBuffer->SetName(L"Vulles Dank:a Task"); //TODO: rename if kept, lol

	D3D12_HEAP_PROPERTIES uploadHeapProperties;
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProperties.VisibleNodeMask = 1;
	uploadHeapProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC uploadDesc;
	uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadDesc.Alignment = 0;
	uploadDesc.Width = (vBufferSize + 255) & ~255; //Bytes!
	uploadDesc.Height = 1;
	uploadDesc.DepthOrArraySize = 1;
	uploadDesc.MipLevels = 1;
	uploadDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadDesc.SampleDesc.Count = 1;
	uploadDesc.SampleDesc.Quality = 0;
	uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pVBUpload;
	this->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pVBUpload));

	pVBUpload->SetName(L"Vulles Dank:a Upload Heap"); //TODO: this one too ex dee

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList);
	vertexData.RowPitch = uploadDesc.Width;
	vertexData.SlicePitch = uploadDesc.Width; //both are supposed to be size in bytes of all triangles...

	UINT64 RequiredSize = 0;
	UINT NumSubresources = 1;
	UINT FirstSubresource = 0;
	UINT64 IntermediateOffset = 0;

	UINT64 MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
	if (MemToAlloc > SIZE_MAX)
	{
		return 0;
	}
	void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
	if (pMem == NULL)
	{
		return 0;
	}
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
	UINT64* pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
	UINT* pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);
	//pVBuffer is dest.
	ID3D12Device* pDevice;
	pVBuffer->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
	pDevice->GetCopyableFootprints(&bufferDesc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
	pDevice->Release();

	if (uploadDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
		uploadDesc.Width < RequiredSize + pLayouts[0].Offset ||
		RequiredSize >(SIZE_T) - 1 ||
		(bufferDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
		(FirstSubresource != 0 || NumSubresources != 1)))
	{
		return 0;
	}

	BYTE* pData;
	HRESULT hr = pVBUpload->Map(0, NULL, reinterpret_cast<void**>(&pData));
	if (FAILED(hr))
	{
		return 0;
	}
	for (UINT i = 0; i < NumSubresources; ++i)
	{
		if (pRowSizesInBytes[i] > (SIZE_T)-1) return 0;
		D3D12_MEMCPY_DEST DestData = { &vertexData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i] };
		for (UINT z = 0; z < pLayouts[i].Footprint.Depth; ++z)
		{
			//BYTE* pDestSlice = reinterpret_cast<BYTE*>(DestData.pData) + DestData.SlicePitch * z;
			const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(vertexData.pData) + vertexData.SlicePitch * z;
			for (UINT y = 0; y < pNumRows[i]; ++y)
			{
				memcpy(pData + DestData.RowPitch * y,
					pSrcSlice + vertexData.RowPitch * y,
					(SIZE_T)pRowSizesInBytes[i]);
			}
		}
	}
	pVBUpload->Unmap(0, NULL);

	D3D12_BOX SrcBox;
	SrcBox.left = UINT(pLayouts[0].Offset);
	SrcBox.right = UINT(pLayouts[0].Offset + pLayouts[0].Footprint.Width);
	SrcBox.top = 0;
	SrcBox.front = 0;
	SrcBox.bottom = 1;
	SrcBox.back = 1;
	pCmdList->CopyBufferRegion(pVBuffer, 0, pVBUpload, pLayouts[0].Offset, pLayouts[0].Footprint.Width);

	D3D12_VERTEX_BUFFER_VIEW vbView;
	vbView.BufferLocation = pVBuffer->GetGPUVirtualAddress();
	vbView.StrideInBytes = sizeof(float4);
	vbView.SizeInBytes = vBufferSize;
	

	//Index buffer-----------------------

	//DWORD iList[] = {
	//	0, 1, 2, // first triangle
	//	0, 3, 1 // second triangle
	//};

	DWORD *iList = plane->GetIndices().data();
	int iBufferSize = sizeof(DWORD) * 6 * size * (size - 1);//sizeof(iList);

	D3D12_HEAP_PROPERTIES iBufferProperties;
	iBufferProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	iBufferProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	iBufferProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	iBufferProperties.VisibleNodeMask = 1;
	iBufferProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC iBufferDesc;
	iBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	iBufferDesc.Alignment = 0;
	iBufferDesc.Width = (iBufferSize + 255) & ~255; //Bytes!
	iBufferDesc.Height = 1;
	iBufferDesc.DepthOrArraySize = 1;
	iBufferDesc.MipLevels = 1;
	iBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	iBufferDesc.SampleDesc.Count = 1;
	iBufferDesc.SampleDesc.Quality = 0;
	iBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	iBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pIBuffer;
	hr = this->GetDevice()->CreateCommittedResource(
		&iBufferProperties,
		D3D12_HEAP_FLAG_NONE, 
		&iBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&pIBuffer));

	if (FAILED(hr))
	{
		return 0;
	}

	pIBuffer->SetName(L"IBuffer Resource Heap");

	D3D12_HEAP_PROPERTIES iUploadHeapProperties;
	iUploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	iUploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	iUploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	iUploadHeapProperties.VisibleNodeMask = 1;
	iUploadHeapProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC iUploadDesc;
	iUploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	iUploadDesc.Alignment = 0;
	iUploadDesc.Width = (iBufferSize + 255) & ~255; //Bytes!
	iUploadDesc.Height = 1;
	iUploadDesc.DepthOrArraySize = 1;
	iUploadDesc.MipLevels = 1;
	iUploadDesc.Format = DXGI_FORMAT_UNKNOWN;
	iUploadDesc.SampleDesc.Count = 1;
	iUploadDesc.SampleDesc.Quality = 0;
	iUploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	iUploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pIBUpload;
	hr = this->GetDevice()->CreateCommittedResource(
		&iUploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&iUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pIBUpload));

	if (FAILED(hr))
	{
		return 0;
	}
	pIBUpload->SetName(L"IBuffer Upload Heap");

	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(iList); // pointer to our index array
	indexData.RowPitch = iBufferSize; // size of all our index buffer
	indexData.SlicePitch = iBufferSize; // also the size of our index buffer

	RequiredSize = 0;
	NumSubresources = 1;
	FirstSubresource = 0;
	IntermediateOffset = 0;

	MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
	if (MemToAlloc > SIZE_MAX)
	{
		return 0;
	}
	pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
	if (pMem == NULL)
	{
		return 0;
	}
	pLayouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
	pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
	pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);
	//pVBuffer is dest.
	pDevice;
	pIBuffer->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
	pDevice->GetCopyableFootprints(&bufferDesc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
	pDevice->Release();

	if (iUploadDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
		iUploadDesc.Width < RequiredSize + pLayouts[0].Offset ||
		RequiredSize >(SIZE_T) - 1 ||
		(iBufferDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
		(FirstSubresource != 0 || NumSubresources != 1)))
	{
		return 0;
	}

	pData;
	hr = pIBUpload->Map(0, NULL, reinterpret_cast<void**>(&pData));
	if (FAILED(hr))
	{
		return 0;
	}
	for (UINT i = 0; i < NumSubresources; ++i)
	{
		if (pRowSizesInBytes[i] >(SIZE_T)-1) return 0;
		D3D12_MEMCPY_DEST DestData = { &indexData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i] };
		for (UINT z = 0; z < pLayouts[i].Footprint.Depth; ++z)
		{
			//BYTE* pDestSlice = reinterpret_cast<BYTE*>(DestData.pData) + DestData.SlicePitch * z;
			const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(indexData.pData) + indexData.SlicePitch * z;
			for (UINT y = 0; y < pNumRows[i]; ++y)
			{
				memcpy(pData + DestData.RowPitch * y,
					pSrcSlice + indexData.RowPitch * y,
					(SIZE_T)pRowSizesInBytes[i]);
			}
		}
	}
	pIBUpload->Unmap(0, NULL);

	D3D12_BOX SrcBox2;
	SrcBox2.left = UINT(pLayouts[0].Offset);
	SrcBox2.right = UINT(pLayouts[0].Offset + pLayouts[0].Footprint.Width);
	SrcBox2.top = 0;
	SrcBox2.front = 0;
	SrcBox2.bottom = 1;
	SrcBox2.back = 1;
	pCmdList->CopyBufferRegion(pIBuffer, 0, pIBUpload, pLayouts[0].Offset, pLayouts[0].Footprint.Width);

	D3D12_INDEX_BUFFER_VIEW ibView;
	ibView.BufferLocation = pIBuffer->GetGPUVirtualAddress();
	ibView.SizeInBytes = iBufferSize;
	ibView.Format = DXGI_FORMAT_R32_UINT;

	//-----------------------------------

	plane->SetVertexBuffer(pVBuffer);
	plane->SetVertexBufferView(vbView);
	plane->SetIndexBuffer(pIBuffer);
	plane->SetIndexBufferView(ibView);

	return plane;
}

