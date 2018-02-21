#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <assert.h>
#include "D3DFactory.h"

struct float4
{
	float x;
	float y;
	float z;
	float w;
};

class BezierClass
{
public:
	//public functions
	BezierClass(/*D3DFactory* pFactory*/);
	~BezierClass();
	void DX12Highway();

private:
	//private functions
	void CalculateBezierVertices();
	

	//private variables
	std::vector<float4> m_pBezierVertices;
	//D3DFactory* m_pFactory;
};