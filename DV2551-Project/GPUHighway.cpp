#include "stdafx.h"
#include "GPUHighway.h"


GPUHighway::GPUHighway(
	D3D12_COMMAND_LIST_TYPE			type, 
	ID3D12CommandQueue*				pCQ, 
	ID3D12CommandAllocator**		ppCAs,
	ID3D12GraphicsCommandList**		ppCLs, 
	unsigned int					iNumberOfCLs, 
	ID3D12Fence**					ppFences,
	unsigned int					iNumberOfFences)
{
	m_type = type;
	m_pCQ = pCQ;

	m_iNumberOfCLs = iNumberOfCLs;
	m_ppCAs = ppCAs;
	m_ppCLs = ppCLs;
	m_ppCLQ = new std::vector<ID3D12CommandList*>[iNumberOfCLs];
	m_pIndexCLQ = new std::vector<unsigned int>[iNumberOfCLs];
	m_pCLLock = new int[iNumberOfCLs];
	for (int i = 0; i < iNumberOfCLs; ++i)
	{
		m_pCLLock[i] = i;
	}

	m_iNumberOfFences = iNumberOfFences;

	m_handleFence = CreateEvent(NULL, NULL, NULL, NULL);
	m_ppFences = ppFences;

	m_pFenceValues = new size_t[iNumberOfFences];
	m_pFenceLocked = new bool[iNumberOfFences];
	for (int i = 0; i < iNumberOfFences; ++i)
	{
		m_pFenceValues[i] = 0;
		m_pFenceLocked[i] = false;
	}
}

GPUHighway::~GPUHighway()
{
	WaitForAllFences();

	delete m_pFenceValues;
	delete m_pFenceLocked;

	for (int i = 0; i < m_iNumberOfFences; ++i)
	{
		SAFE_RELEASE(m_ppFences[i]);
	}
	for (int i = 0; i < m_iNumberOfCLs; ++i)
	{
		SAFE_RELEASE(m_ppCAs[i]);
		SAFE_RELEASE(m_ppCLs[i])
	}
}

ID3D12CommandQueue* GPUHighway::GetCQ()
{
	return m_pCQ;
}

void GPUHighway::QueueCL(ID3D12GraphicsCommandList* pCL)
{
	int iLock = -1;
	for (int i = 0; i < m_iNumberOfCLs; ++i)
	{
		if (m_ppCLs[i] == pCL)
		{
			iLock = i;
			m_pCLLock[i] = -1;
		}
	}
	assert(iLock > -1);//cant be queued in this highway

	for (int i = 0; i < m_iNumberOfFences; ++i)
	{
		if (!m_pFenceLocked[i])
		{
			m_ppCLQ[i].push_back(pCL);
			m_pIndexCLQ->push_back(iLock);
		}
	}
}

ID3D12GraphicsCommandList* GPUHighway::GetFreshCL()
{
	ID3D12GraphicsCommandList* pCL = nullptr;
	for (int i = 0; i < m_iNumberOfCLs; ++i)
	{
		if (m_pCLLock[i] == i)
		{
			pCL = m_ppCLs[i];
			pCL->Reset(m_ppCAs[i], nullptr);
			break;
		}
	}
	
	return pCL;

}

int GPUHighway::ExecuteCQ()
{
	int index = -1;
	for (int i = 0; i < m_iNumberOfFences; ++i)
	{
		if (!m_pFenceLocked[i])
		{
			m_pCQ->ExecuteCommandLists(m_ppCLQ[i].size(), m_ppCLQ[i].data());
			m_ppCLQ[i].clear();
		}
	}
	
	for (int i = 0; i < m_iNumberOfFences; ++i)
	{
		if (!m_pFenceLocked[i])
		{
			m_pCQ->Signal(m_ppFences[i], m_pFenceValues[i]);
			index = i;
			m_pFenceLocked[i] = true;
			break;
		}
	}
	return index;
}

void GPUHighway::Wait(int index)
{
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
	for (int i = 0; i < m_iNumberOfFences; ++i)
	{
		if (m_pFenceLocked[i])
		{
			Wait(i);
		}
	}
}


