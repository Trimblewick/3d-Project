#pragma once

#include <Windows.h>
#include <string>


class Window
{
private:
	unsigned long m_iWidth;
	unsigned long m_iHeight;
	float m_fAspectRatio;
	WNDCLASSEX m_info;
	HWND m_hwnd;
	HINSTANCE m_hinstance;
	RECT m_rect;

public:
	Window();
	~Window();

	bool Initialize(HINSTANCE hinstance, int iCmdShow, LONG iWidth, LONG iHeight, WNDPROC EventHandler);
	int GetWidth();
	int GetHeight();
	HWND GetWindowHandle();
	HINSTANCE GetWindowInstance();
	void SetTitle(std::string sTitle);
};







