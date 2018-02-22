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

GPUHighway * D3DFactory::CreateGPUHighway(D3D12_COMMAND_LIST_TYPE type, unsigned int iNumberOfCAs, unsigned int iNumberOfCLs)
{
	if (iNumberOfCAs < iNumberOfCLs)
	{
		return nullptr;
	}
	ID3D12CommandQueue* pCQ = CreateCQ(type);

	std::vector<ID3D12CommandAllocator*> ppCAs;
	std::vector<ID3D12Fence*> ppFences;
	std::vector<ID3D12GraphicsCommandList*> ppCLs;

	for (int i = 0; i < iNumberOfCAs; ++i)
	{
		ppCAs.push_back(CreateCA(type));
		ppFences.push_back(CreateFence());
	}
	for (int i = 0; i < iNumberOfCLs; ++i)
	{
		ppCLs.push_back(CreateCL(ppCAs[i], type));
		DxAssert(ppCLs[i]->Close());
	}


	return new GPUHighway(type, pCQ, ppCAs.data(), ppFences.data(), iNumberOfCAs, ppCLs.data(), iNumberOfCLs);
}

Plane * D3DFactory::CreatePlane(ID3D12GraphicsCommandList* pCmdList)
{
	int size = 16;

	struct Vertex
	{
		float4 position;
		float4 color;
	};
	Vertex vList[] = {
		{ -0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f }
	};
	int vBufferSize = sizeof(vList);

	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.VisibleNodeMask = 1;
	heapProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC bufferDesc;
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;
	bufferDesc.Width = vBufferSize; //Bytes!
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
		D3D12_RESOURCE_STATE_GENERIC_READ,
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
	uploadDesc.Width = vBufferSize; //Bytes!
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

	pVBUpload->SetName(L"Vulles Dank:a Upload Heap");

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList);
	vertexData.RowPitch = vBufferSize;
	vertexData.SlicePitch = vBufferSize; //both are supposed to be size in bytes of all triangles...

	UINT64 RequiredSize = 0;
	UINT64 MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * 1; //1 subresource
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
	UINT64* pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + 1); //1 subresource
	UINT* pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + 1); //1 subresource
	//pVBuffer is dest.
	ID3D12Device* pDevice;
	pVBuffer->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
	//TODO: testa om behövs
	pDevice->GetCopyableFootprints(&bufferDesc, 0/*First subresource*/, 1/*1 subresource*/, 0/*Intermediate (upload heap) offset*/, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
	pDevice->Release();

	/*if (uploadDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
		uploadDesc.Width < RequiredSize + pLayouts[0].Offset ||
		RequiredSize >(SIZE_T) - 1 ||
		(bufferDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
		(FirstSubresource != 0 || NumSubresources != 1)))
	{
		return 0;
	}*/

	/*BYTE* pData;
	HRESULT hr = pIntermediate->Map(0, NULL, reinterpret_cast<void**>(&pData));
	if (FAILED(hr))
	{
		return 0;
	}*/
	UINT NumSubresources = 1;
	for (UINT i = 0; i < NumSubresources; ++i)
	{
		if (pRowSizesInBytes[i] >(SIZE_T)-1) return 0;
		D3D12_MEMCPY_DEST DestData = { &vertexData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i] };
		//MemcpySubresource(&DestData, &pSrcData[i]/*vertexData*/, (SIZE_T)pRowSizesInBytes[i]/*rowsizeinbytes*/, pNumRows[i]/*rows*/, pLayouts[i].Footprint.Depth /*slices*/);
		for (UINT z = 0; z < pLayouts[i].Footprint.Depth; ++z)
		{
			BYTE* pDestSlice = reinterpret_cast<BYTE*>(DestData.pData) + DestData.SlicePitch * z;
			const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(vertexData.pData) + vertexData.SlicePitch * z;
			for (UINT y = 0; y < pNumRows[i]; ++y)
			{
				memcpy(pDestSlice + DestData.RowPitch * y,
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
	vbView.StrideInBytes = sizeof(Vertex);
	vbView.SizeInBytes = vBufferSize;
	

	return new Plane(16, pVBuffer, vbView);
	//ID3D12DescriptorHeap* pDHverts = this->CreateDH(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

}

