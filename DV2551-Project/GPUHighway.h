#pragma once

#include <d3d12.h>
#include <vector>

class GPUHighway
{
private:

public:
	GPUHighway(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandQueue * pCQ, ID3D12CommandAllocator ** ppCAs, 
		ID3D12GraphicsCommandList ** ppCLs, unsigned int iNumberOfCLs, ID3D12Fence** ppFences,
		unsigned int iNumberOfFences);
	~GPUHighway();

	ID3D12CommandQueue*						GetCQ();
	//std::pair<ID3D12CommandList*, ID3D12CommandAllocator*>

	void									QueueCL(ID3D12GraphicsCommandList* pCL);//Make sure only to queue commandlists on the same highway. close list aswell
	ID3D12GraphicsCommandList*				GetFreshCL();
	
	int										ExecuteCQ();//Execute all queued commandlists. Returns the index of the fence used.
	void									Wait(int index);//Wait for a particular fence using a index. Returns -1 if no free fences was found.
	void									WaitForAllFences();//Full reset, all fences will be waited for.

private:
	D3D12_COMMAND_LIST_TYPE					m_type;
	ID3D12CommandQueue*						m_pCQ;

	ID3D12Fence**							m_ppFences;
	unsigned int							m_iNumberOfFences; 
	size_t*									m_pFenceValues;
	bool*									m_pFenceLocked;

	HANDLE									m_handleFence;
	
	ID3D12CommandAllocator**				m_ppCAs;
	ID3D12GraphicsCommandList**				m_ppCLs;
	unsigned int							m_iNumberOfCLs;
	int*									m_pCLLock;
	std::vector<ID3D12CommandList*>*		m_ppCLQ;
	std::vector<unsigned int>*				m_pIndexCLQ;
};

