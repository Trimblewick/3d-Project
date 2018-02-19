#include "stdafx.h"
#include "Input.h" 

Input::Input()
{

}

Input::~Input()
{
}


bool Input::Initialize()
{
	for (unsigned int i = 0; i < 256; ++i)
	{
		m_bKeys[i] = false;
	}

	return true;
}


void Input::KeyDown(unsigned int const iKeyIndex)
{
	if (iKeyIndex >= 0 && iKeyIndex <= 256)
		m_bKeys[iKeyIndex] = true;
	
	return;
}


void Input::KeyUp(unsigned int const iKeyIndex)
{
	if (iKeyIndex >= 0 && iKeyIndex <= 256)
		m_bKeys[iKeyIndex] = false;
	
	return;
}


const bool Input::IsKeyDown(unsigned int const iKeyIndex)
{
	return m_bKeys[iKeyIndex];
}

