#include <windows.h>
#include "win.h"
#include "sample/sample.h"
#include <filesystem>

#include <io.h>
#include <fcntl.h>
#include <cstdio>

std::filesystem::path get_exe_path() {
	char buffer[MAX_PATH];
	GetModuleFileName(nullptr, buffer, sizeof(buffer));
	return std::filesystem::path(buffer).parent_path();
}

void redirect_stdout() {
	if(AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	redirect_stdout();
	auto exe_path = get_exe_path();
	auto model_path = exe_path / "model" / "googlenet-v1" / "FP32" / "googlenet-v1.xml";
	auto image_path = exe_path / "image" / "car.bmp";

	sample(
		model_path.string(),
		image_path.string(),
		"GPU",
		"C:\\crash_test_cl_cache\\"
	);

	Win win(hInstance, "MainWindow", "Win32");
	return win.Run()? 0 : 1;
}
