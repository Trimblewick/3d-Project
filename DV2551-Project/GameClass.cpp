#include "stdafx.h"

#include "GameClass.h"


GameClass::GameClass()
{
	
}

GameClass::~GameClass()
{
	
}

bool GameClass::Initialize()
{
	m_pD3DFactory = new D3DFactory();

	return true;
}

void GameClass::CleanUp()
{
	if (m_pD3DFactory)
	{
		delete m_pD3DFactory;
		m_pD3DFactory = nullptr;
	}
}

void GameClass::Update(Input * input, double dDeltaTime)
{
	m_dDeltaTime = dDeltaTime;
}

void GameClass::ClearBackBuffer()
{


}

void GameClass::PrecentBackBuffer()
{


}

void GameClass::Frame()
{


}
