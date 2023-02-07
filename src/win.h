#pragma once

#include <windows.h>
#include <string>


/// A dummy blank window.
class Win
{
public:
	Win(HINSTANCE hInstance, std::string winClass, std::string title);
	bool Run();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE _hInstance;
	HWND _hwnd;
	std::string _winClass;
	std::string _title;
};