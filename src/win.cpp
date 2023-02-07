#include "win.h"

Win::Win(HINSTANCE hInstance, std::string winClass, std::string title)
	: _hInstance{hInstance}, _hwnd{nullptr}, _winClass{winClass}, _title{title}
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _winClass.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(
			NULL,
			"Call to RegisterClassEx failed!",
			"Windows Desktop Guided Tour",
			NULL);
	}

	_hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		_winClass.c_str(),
		_title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL,
		NULL,
		_hInstance,
		NULL);
}

bool Win::Run()
{
	if (!_hwnd)
	{
		MessageBox(
			NULL,
			"Call to CreateWindow failed!",
			"Win32",
			NULL
		);

		return false;
	}

	ShowWindow(_hwnd, SW_SHOW);
	UpdateWindow(_hwnd);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

LRESULT CALLBACK Win::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	std::string text {"This window intentionally left blank."};
	HWND hwndButton = nullptr;

	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		TextOut(
			hdc,
			5, 5,
			text.c_str(), text.size()
		);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}