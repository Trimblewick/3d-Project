#include "stdafx.h"
#include "BezierClass.h"

//TODO: move root constants/paramters etc to factory, 
//		make a bonstantCuffer function here that sends m_pBezierVertices to GPU to be used for offset calc,
//		fill RootSig in D3DFactory
//		set PSO in D3DFactory

BezierClass::BezierClass(/**/)
{
	float4 temp;
	temp.x = 1.0f;
	temp.y = 0.0f;
	temp.z = 0.0f;
	temp.w = 0.0f;

	m_pBezierVertices.push_back(temp);
	m_pPreviouslyCalculatedBezierVertices.push_back(temp);
}

BezierClass::~BezierClass()
{
	m_pBezierVertices.clear();
	m_pPreviouslyCalculatedBezierVertices.clear();
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pConstantUploadHeap);
	//DELET THIS, hihi
}

void BezierClass::bezierConstantBuffer(std::vector<float4> vertices)
{
	//upload calculated vertices to gpu, will be called from GameClass every frame
	
}

void BezierClass::CalculateBezierVertices()
{
	int nrOfVertices = 1; //temp, set to number of vertices so we can loop for each vertex and calculate bezier offset

	for (int i = 0; i < nrOfVertices; i++)
	{
		//Change line below to actual Bézier calculationusing previous frame's bezier point
		float4 previousBezierWithOffset = m_pPreviouslyCalculatedBezierVertices[i]; //previousBezierWithOffset will be m_pPreviouslyCalculatedBezierVertices[i] with an offset

		m_pBezierVertices.push_back(previousBezierWithOffset);
		m_pPreviouslyCalculatedBezierVertices[i] = previousBezierWithOffset; //update for next frame
	}

	bezierConstantBuffer(m_pBezierVertices);
	m_pPreviouslyCalculatedBezierVertices = m_pBezierVertices;
	return;
}
