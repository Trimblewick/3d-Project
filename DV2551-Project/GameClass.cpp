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
	return true;
}

void GameClass::CleanUp()
{
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
