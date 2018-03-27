#include "stdafx.h"
#include "SystemClass.h"
#include <fstream>

Window SystemClass::s_window;
GameClass SystemClass::s_game;
Input SystemClass::s_input;

bool SystemClass::s_bRunning;
bool SystemClass::s_bInitialized;
double SystemClass::s_fDeltaTime;


LRESULT CALLBACK SystemClass::EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
			}
		}
		else
		{
			s_input.KeyDown((unsigned int)wParam);
		}
		return 0;

	case WM_KEYUP:
		s_input.KeyUp((unsigned int)wParam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

SystemClass::SystemClass()
{
	s_bRunning = false;
	s_bInitialized = false;
	s_fDeltaTime = 0.0f;
}

SystemClass::~SystemClass()
{
}



bool SystemClass::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int iCmdShow, LONG windowWidth, LONG windowHeight)
{
	if (s_bInitialized)
	{
		return false;
	}
	if (!s_window.Initialize(hInstance, iCmdShow, windowWidth, windowHeight, EventHandler))
	{
		return false;
	}
	if (!s_game.Initialize(&s_window))
	{
		return false;
	}
	if (!s_input.Initialize())
	{
		return false;
	}
	
	s_bRunning = true;
	s_bInitialized = true;
	return true;
}


void SystemClass::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	srand(time(NULL));
	auto prevTime = std::chrono::steady_clock::now();
	auto currentTime = std::chrono::steady_clock::now();
	int smoothFPSCounter = 0;
	int addFPS = 0;
	float fFPS = 0;
	float sumFPS = 0;
	float avgFPS = 0;
	float second = 0;
	int seconds = 0;
	std::ofstream outFile;
	outFile.open("OneDirect.txt");
	while (s_bRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				SystemClass::Stop();
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else //loop logics
		{
			auto currentTime = std::chrono::steady_clock::now();
			s_fDeltaTime = (currentTime - prevTime).count() / 1000000000.0f;
			prevTime = currentTime;

			second += s_fDeltaTime;
			fFPS = 1.0f / s_fDeltaTime;
			sumFPS += fFPS;
			addFPS++;
			if (second > 1)
			{
				seconds++;
				avgFPS = sumFPS / addFPS;
				outFile << seconds << '\t' << avgFPS << '\n';
				addFPS = 0;
				second = 0;
				sumFPS = 0;
			}
			
			if (seconds == 30)
			{
				break;
			}
			/*fFPS = fFPS;
			if (smoothFPSCounter % 512 == 0 && smoothFPSCounter != 0)
			{
				fFPS = addFPS / 512;
				addFPS = 0;
			}
			addFPS += 1.0f / s_fDeltaTime;
			smoothFPSCounter += 1;*/

			std::string sFPS = "FPS: " + std::to_string(avgFPS);
			s_window.SetTitle(sFPS);



			//basic flow
			s_game.Update(&s_input, s_fDeltaTime);
			//s_game.ClearBackBuffer();
			//s_game.Frame();
			//s_game.PrecentBackBuffer();

			
		}
	}
	
	outFile.close();
}

void SystemClass::Pause()
{
}

void SystemClass::Stop()
{
	s_bRunning = false;
}

void SystemClass::CleanUp()
{
	s_game.CleanUp();
	
}
