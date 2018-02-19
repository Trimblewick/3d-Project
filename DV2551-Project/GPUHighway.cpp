#include "stdafx.h"
#include "GPUHighway.h"


GPUHighway::GPUHighway(
	D3D12_COMMAND_LIST_TYPE			type, 
	ID3D12CommandQueue*				pCQ,
	ID3D12CommandAllocator**		ppCAs, 
	ID3D12Fence**					ppFences, 
	unsigned int					iNumberOfCAsAndFences, 
	ID3D12GraphicsCommandList**		ppCLs, 
	unsigned int					iNumberOfCLs)
{
	assert(iNumberOfCAsAndFences >= iNumberOfCLs);
	m_type = type;
	m_pCQ = pCQ;
	
	m_ppCAs = ppCAs;
	m_ppFences = ppFences;
	m_iNumberOfCAsAndFences = iNumberOfCAsAndFences;

	m_pFenceValues = new size_t[iNumberOfCAsAndFences];
	m_ppCAVec = new std::vector<ID3D12CommandAllocator*>[iNumberOfCAsAndFences];
	m_pFenceLocked = new bool[iNumberOfCAsAndFences];

	m_handleFence = CreateEvent(NULL, NULL, NULL, NULL);

	for (int i = 0; i < iNumberOfCAsAndFences; ++i)
	{
		m_pFenceValues[i] = 0;
		m_pFenceLocked[i] = false;
		
	}

	m_ppCLs = ppCLs;
	m_iNumberOfCLs = iNumberOfCLs;
	m_pCLLocked = new bool[iNumberOfCLs];
	for (int i = 0; i < iNumberOfCLs; ++i)
	{
		m_pCLLocked = false;
	}
}

GPUHighway::~GPUHighway()
{
	delete m_pFenceValues;

	for (int i = 0; i < m_iNumberOfCAsAndFences; ++i)
	{
		
	}

}

ID3D12CommandQueue * GPUHighway::GetCQ()
{
	return m_pCQ;
}

void GPUHighway::QueueCL(ID3D12CommandList* pCL)
{
	m_ppCLQ.push_back(pCL);
}

ID3D12GraphicsCommandList * GPUHighway::GetFreshCL()
{
	return nullptr;
}

int GPUHighway::ExecuteCQ()
{
	m_pCQ->ExecuteCommandLists(m_ppCLQ.size(), m_ppCLQ.data());
	m_ppCLQ.clear();

	int index = -1;
	for (int i = 0; i < m_iNumberOfCAsAndFences; ++i)//use the first fence that is availible
	{ 
		if (!m_pFenceLocked[i])
		{
			m_pCQ->Signal(m_ppFences[i], m_pFenceValues[i]);
			index = i;
			i = m_iNumberOfCAsAndFences;
		}
	}
	return index;
}

void GPUHighway::Wait(int index)
{
	if (!m_pFenceLocked)
		return;

	if (m_ppFences[index]->GetCompletedValue() < m_pFenceValues[index])
	{
		m_ppFences[index]->SetEventOnCompletion(m_pFenceValues[index], m_handleFence);
		WaitForSingleObject(m_handleFence, INFINITE);
	}
	m_pFenceValues[index]++;
	m_pFenceLocked[index] = false;

	//reset CAs
	DxAssert(m_ppCAs[index]->Reset());
}

void GPUHighway::WaitForAllFences()
{
	for (int i = 0; i < m_iNumberOfCAsAndFences; ++i)
	{
		if (m_pFenceLocked[i])
		{
			Wait(i);
		}
	}
}


