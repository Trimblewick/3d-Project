#include "stdafx.h"
#include "GPUHighway.h"


GPUHighway::GPUHighway(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandQueue* pCQ, ID3D12CommandAllocator** ppCAs,
	ID3D12GraphicsCommandList** ppCLs, ID3D12Fence** ppFences, unsigned int iNumberOfCLs)
{
	m_type = type;
	m_pCQ = pCQ;

	m_iNumberOfCLs = iNumberOfCLs;
	m_ppCAs = ppCAs;
	m_ppCLs = ppCLs;
	m_ppFences = ppFences;
	m_ppCLQ = new std::vector<ID3D12CommandList*>[iNumberOfCLs];
	m_pIndexCLQ = new std::vector<unsigned int>[iNumberOfCLs];
	m_pCLLock = new int[iNumberOfCLs];
	m_pFenceValues = new size_t[iNumberOfCLs];
	m_pFenceLocked = new bool[iNumberOfCLs];

	for (unsigned int i = 0; i < iNumberOfCLs; ++i)
	{
		m_pCLLock[i] = i;
		m_pFenceValues[i] = 0;
		m_pFenceLocked[i] = false;
	}

	m_handleFence = CreateEvent(NULL, NULL, NULL, NULL);
}

GPUHighway::~GPUHighway()
{
	WaitForAllFences();

	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)
	{
		SAFE_RELEASE(m_ppCAs[i]);
		SAFE_RELEASE(m_ppCLs[i]);
		SAFE_RELEASE(m_ppFences[i]);
	}
	SAFE_RELEASE(m_pCQ);

	delete[] m_ppCLs;
	delete[] m_ppCAs;
	delete[] m_ppFences;
	delete m_pCLLock;
	delete m_pFenceValues;
	delete m_pFenceLocked;
	delete[] m_pIndexCLQ;
	delete[] m_ppCLQ;
}

ID3D12CommandQueue* GPUHighway::GetCQ()
{
	return m_pCQ;
}

void GPUHighway::QueueCL(ID3D12GraphicsCommandList*& pCL)
{
	int iLock = -1;
	
	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)//error handling
	{
		if (m_ppCLs[i] == pCL)
		{
			iLock = i;
			m_pCLLock[i] = -1;
		}
	}
	if (iLock == -1)//cant be queued in this highway
	{
		OutputDebugString(L"Address to commandlist does not belong in this highway\n");
		return;
	}

	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)//find free fence
	{
		if (!m_pFenceLocked[i])
		{
			pCL->Close();
			m_ppCLQ[i].push_back(pCL);
			m_pIndexCLQ[i].push_back(iLock);
			pCL = nullptr;
			break;
		}
	}
}

ID3D12GraphicsCommandList* GPUHighway::GetFreshCL(ID3D12PipelineState* pPSO)
{
	ID3D12GraphicsCommandList* pCL = nullptr;
	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)
	{
		if (m_pCLLock[i] == i)
		{
			pCL = m_ppCLs[i];
			pCL->Reset(m_ppCAs[i], pPSO);
			break;
		}
	}
	
	return pCL;

}

int GPUHighway::ExecuteCQ()
{
	int index = -1;
	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)
	{
		if (!m_pFenceLocked[i])
		{
			m_pCQ->ExecuteCommandLists((unsigned int)m_ppCLQ[i].size(), m_ppCLQ[i].data());
			m_ppCLQ[i].clear();
		}
	}
	
	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)
	{
		if (!m_pFenceLocked[i])//find first free fence
		{
			m_pCQ->Signal(m_ppFences[i], m_pFenceValues[i]);//Maybe signal in Wait() instead?
			index = i;
			m_pFenceLocked[i] = true;
			break;
		}
	}
	return index;
}

void GPUHighway::Wait(int index)
{
	if (index < 0 || index >= m_iNumberOfCLs)
		return;
	if (!m_pFenceLocked[index])
		return;

	if (m_ppFences[index]->GetCompletedValue() < m_pFenceValues[index])
	{
		m_ppFences[index]->SetEventOnCompletion(m_pFenceValues[index], m_handleFence);
		WaitForSingleObject(m_handleFence, INFINITE);
	}
	m_pFenceValues[index]++;
	m_pFenceLocked[index] = false;

	for (unsigned int i : m_pIndexCLQ[index])
	{
		m_ppCAs[i]->Reset();
		m_pCLLock[i] = i;
	}
	m_pIndexCLQ[index].clear();
}

void GPUHighway::WaitForAllFences()
{
	for (unsigned int i = 0; i < m_iNumberOfCLs; ++i)
	{
		if (m_pFenceLocked[i])
		{
			Wait(i);
		}
	}
}


