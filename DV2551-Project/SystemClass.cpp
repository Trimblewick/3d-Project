#include "stdafx.h"
#include "SystemClass.h"


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
			
			if(smoothFPSCounter == 0)
				fFPS = 1.0f / s_fDeltaTime;

			fFPS = fFPS;
			if (smoothFPSCounter % 512 == 0 && smoothFPSCounter != 0)
			{
				fFPS = addFPS / 512;
				addFPS = 0;
			}
			addFPS += 1.0f / s_fDeltaTime;
			smoothFPSCounter += 1;

			std::string sFPS = "FPS: " + std::to_string(fFPS);
			s_window.SetTitle(sFPS);



			//basic flow
			s_game.Update(&s_input, s_fDeltaTime);
			//s_game.ClearBackBuffer();
			//s_game.Frame();
			//s_game.PrecentBackBuffer();

			
		}
	}
	
	
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
