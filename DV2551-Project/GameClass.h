#pragma once

#include "Input.h"

class GameClass
{
private:

public:
	GameClass();
	~GameClass();

	bool Initialize();
	void CleanUp();

	void Update(Input* input, double dDeltaTime);
	void ClearBackBuffer();
	void PrecentBackBuffer();
	void Frame();


	

private:
	double m_dDeltaTime;
};
 
