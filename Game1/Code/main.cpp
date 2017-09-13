#include <windows.h>
#include <Game/App.h>
#include "Util/fft.h"

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;
	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


int WINAPI wWinMain(HINSTANCE appInstance, HINSTANCE prevInstane, LPWSTR cmdLine, int cmdShow)
{
	(void)cmdLine;
	(void)prevInstane;
	//bool fftCorrect = Util::testFFT<1024>();
	//if (!fftCorrect)
	//{
	//	return -1;
	//}
	//(void)fftCorrect;
	WNDCLASSEX windowDesc = { 0 };
	windowDesc.cbSize = sizeof(WNDCLASSEX);
	windowDesc.style = CS_HREDRAW | CS_VREDRAW;
	windowDesc.lpfnWndProc = wndProc;
	windowDesc.hInstance = appInstance;
	windowDesc.hCursor = LoadCursor(NULL, IDC_CROSS);
	windowDesc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowDesc.lpszMenuName = NULL;
	windowDesc.lpszClassName = "GameWindowDescriptor";
	if (!RegisterClassEx(&windowDesc))
	{
		return -1;
	}
	RECT rc = { 0, 0, 852, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE );
	HWND windowHandle = CreateWindowA(
		"GameWindowDescriptor",
		"Game",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, NULL, appInstance, NULL
		);
	if (!windowHandle)
	{
		return -1;
	}
	ShowWindow(windowHandle, cmdShow);
	ShowCursor(false);
	MSG message = { 0 };

	Game::App app;
	app.start(appInstance, windowHandle);

	while (message.message != WM_QUIT && ! app.isExitTriggered())
	{
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		app.nextFrame();
		
	}
	app.stop();
	return 0;
}