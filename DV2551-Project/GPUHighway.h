#pragma once

#include <d3d12.h>
#include <vector>

class GPUHighway
{
public:
	GPUHighway(ID3D12CommandQueue* pCQ, ID3D12CommandAllocator** ppCAs, unsigned int iNumberOfCAs, ID3D12GraphicsCommandList** ppCLs, unsigned int iNumberOfCLs);
	~GPUHighway();

	ID3D12CommandQueue*						GetCQ();

	ID3D12GraphicsCommandList*				GetFreeCL();
	
	void									ExecuteCQ(ID3D12Fence* pFence, const size_t iValue);
private:
	D3D12_COMMAND_LIST_TYPE					m_type;
	
	ID3D12CommandQueue*						m_pCQ;

	ID3D12CommandAllocator**				m_ppCAs;
	unsigned int							m_iNumberOfCAs; 
	ID3D12GraphicsCommandList**				m_ppCLs;
	unsigned int							m_iNumberOfCLs;

	std::vector<ID3D12CommandList*>			m_ppCLQ;
};

