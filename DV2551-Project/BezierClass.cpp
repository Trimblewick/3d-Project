#include "stdafx.h"
#include "BezierClass.h"

//TODO: Quick test bind cbuffer to shader and see if values get through 
//		memcpy new cbuffer data in update loop every frame to cbuffer??
//		
//		

BezierClass::BezierClass(/**/)
{
}

BezierClass::BezierClass(ID3D12DescriptorHeap * pDH, ID3D12Resource* pResource)
{
	float4 temp;
	temp.x = 1.0f;
	temp.y = 0.0f;
	temp.z = 0.0f;
	temp.w = 0.0f;

	m_nrOfVertices = 0; //update

	m_pBezierVertices.push_back(temp);
	m_pPreviouslyCalculatedBezierVertices.push_back(temp);

	m_pConstantDescHeap = pDH;
	m_pConstantUploadHeap = pResource;
}

BezierClass::~BezierClass()
{
	m_pBezierVertices.clear();
	m_pPreviouslyCalculatedBezierVertices.clear();
	SAFE_RELEASE(m_pConstantDescHeap);
	SAFE_RELEASE(m_pConstantUploadHeap);
	//DELET THIS, hihi
}

void BezierClass::CalculateBezierVertices(/*&commandList*/)
{
	int nrOfVertices = 1; //temp, set to number of vertices so we can loop for each vertex and calculate bezier offset

	for (int i = 0; i < nrOfVertices; i++)
	{
		//Change line below to actual Bézier calculationusing previous frame's bezier point
		float4 previousBezierWithOffset = m_pPreviouslyCalculatedBezierVertices[i]; //previousBezierWithOffset will be m_pPreviouslyCalculatedBezierVertices[i] with an offset

		m_pBezierVertices.push_back(previousBezierWithOffset);
		m_pPreviouslyCalculatedBezierVertices[i] = previousBezierWithOffset; //update for next frame
	}

	m_pPreviouslyCalculatedBezierVertices = m_pBezierVertices;

	//update commandList with new vertices from m_pBezierVertices
	return;
}
