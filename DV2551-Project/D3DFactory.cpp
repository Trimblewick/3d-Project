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
