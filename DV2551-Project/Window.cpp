#include "stdafx.h"
#include "Window.h"


Window::Window()
{

}

Window::~Window()
{

}

bool Window::Initialize(HINSTANCE hinstance, int iCmdShow, LONG iWidth, LONG iHeight, WNDPROC EventHandler)
{
	m_hinstance = hinstance;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_info = { 0 };
	m_info.cbSize = sizeof(WNDCLASSEX);
	m_info.style = CS_HREDRAW | CS_VREDRAW;
	m_info.lpfnWndProc = EventHandler;
	m_info.hInstance = hinstance;
	m_info.lpszClassName = L"Window";
	m_info.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(101));

	if (!RegisterClassEx(&m_info))
		return false;

	m_rect = { 0, 0, iWidth, iHeight };

	bool test = AdjustWindowRect(&m_rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hwnd = CreateWindow(
		L"Window",			
		L"Window",		
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_rect.right - m_rect.left,
		m_rect.bottom - m_rect.top,
		nullptr,
		nullptr,
		hinstance,
		nullptr);

	if (m_hwnd)
	{
		ShowWindow(m_hwnd, iCmdShow);
	}

	return true;
}

int Window::GetWidth()
{
	return m_iWidth;
}

int Window::GetHeight()
{
	return m_iHeight;
}

HWND Window::GetWindowHandle()
{
	return m_hwnd;
}

HINSTANCE Window::GetWindowInstance()
{
	return m_hinstance;
}

void Window::SetTitle(std::string sTitle)
{
	std::wstring wstemp = std::wstring(sTitle.begin(), sTitle.end());
	LPCWSTR title = wstemp.c_str();
	SetWindowText(m_hwnd, title);
}
