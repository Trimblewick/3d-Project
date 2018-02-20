#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <assert.h>

//#include "stdafx.h"
//calculates Bezier
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
	void SetRootDesc();

private:
	//private functions
	void CalculateBezierVertices();
	

	//private variables
	std::vector<float4> m_pBezierVertices;
};