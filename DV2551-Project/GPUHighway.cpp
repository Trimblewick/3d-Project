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
	m_type = type;
	m_pCQ = pCQ;
	
	m_ppCAs = ppCAs;
	m_ppFences = ppFences;
	m_iNumberOfCAsAndFences = iNumberOfCAsAndFences;

	m_pFenceValues = new size_t[iNumberOfCAsAndFences];
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

void GPUHighway::QueueCL(ID3D12GraphicsCommandList* pCL)
{
	pCL->Close();
	m_ppCLQ.push_back(pCL);
}

ID3D12GraphicsCommandList * GPUHighway::GetFreshCL()
{
	Command newCommand;
	for (int i = 0; i < m_iNumberOfCLs; ++i)//find free cl
	{
		if (!m_pCLLocked[i])
		{
			newCommand.pCL = m_ppCLs[i];
			i = m_iNumberOfCLs;//exit
		}
	}

	if (newCommand.pCL == nullptr)//need more cls
		return nullptr;

	int iSizeVec = m_commandVec.size();

	if (iSizeVec >= m_iNumberOfCAsAndFences)
		return nullptr;

	for (int i = 0; i < iSizeVec; ++i)//find free ca
	{
		if (m_ppCAs[i] != m_commandVec[i].pCA)
		{
			newCommand.pCA = m_ppCAs[i];
			i = iSizeVec;//exit
		}
	}

	m_commandVec.push_back(newCommand);
	newCommand.pCL->Reset(newCommand.pCA, nullptr);

	return newCommand.pCL;
}

int GPUHighway::ExecuteCQ()
{
	m_pCQ->ExecuteCommandLists(m_ppCLQ.size(), m_ppCLQ.data());

	for (Command c : m_commandVec)
	{
		for (ID3D12CommandList* p : m_ppCLQ)
		{
			if (c.pCL == p)
			{
				c.pCL = nullptr;
				break;
			}
		}
	}
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
	for (int i = 0; i < m_commandVec.size(); ++i)
	{
		if (m_commandVec[i].pCL = nullptr)
		{
			DxAssert(m_commandVec[i].pCA->Reset());
			m_commandVec[i].pCA = nullptr;
			m_commandVec.erase(m_commandVec.begin() + i, m_commandVec.begin() + i + 1);
		}
	}
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


