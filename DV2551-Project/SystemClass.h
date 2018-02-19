#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers.
#endif

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include <chrono>
#include <string>


#include "Window.h"
#include "GameClass.h"
#include "Input.h"

class SystemClass
{
private:
	static LRESULT CALLBACK EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	SystemClass();
	~SystemClass();

	static bool Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int iCmdShow, LONG windowWidth, LONG windowHeight);
	static void Run();
	static void Pause();
	static void Stop();
	static void CleanUp();


private:

	static Window s_window;
	static GameClass s_game;
	static Input s_input;

	static bool s_bRunning;
	static bool s_bInitialized;
	static float s_fDeltaTime;
};
