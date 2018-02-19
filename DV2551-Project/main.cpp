#include "stdafx.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 

#include "SystemClass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	MSG msg = { 0 };

	SystemClass::Initialize(hInstance, hPrevInstance, nShowCmd, (long)800, (long)640);
	SystemClass::Run();
	SystemClass::CleanUp();

	_CrtDumpMemoryLeaks();
	return (int)msg.wParam;
}
