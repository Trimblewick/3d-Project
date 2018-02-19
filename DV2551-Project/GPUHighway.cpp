#include "stdafx.h"
#include "GPUHighway.h"


GPUHighway::GPUHighway(ID3D12CommandQueue * pCQ, ID3D12CommandAllocator ** ppCAs, unsigned int iNumberOfCAs, ID3D12GraphicsCommandList ** ppCLs, unsigned int iNumberOfCLs)
{
	m_pCQ = pCQ;
	m_ppCAs = ppCAs;
	m_iNumberOfCAs = iNumberOfCAs;
	m_ppCLs = ppCLs;
	m_iNumberOfCLs = iNumberOfCLs;
}

GPUHighway::~GPUHighway()
{
}

ID3D12CommandQueue * GPUHighway::GetCQ()
{
	return m_pCQ;
}

ID3D12GraphicsCommandList * GPUHighway::GetFreeCL()
{
	return nullptr;
}

void GPUHighway::ExecuteCQ(ID3D12Fence* pFence, const size_t iValue)
{
	m_pCQ->ExecuteCommandLists(m_ppCLQ.size(), m_ppCLQ.data());
	m_ppCLQ.clear();

	m_pCQ->Signal(pFence, iValue);
}
