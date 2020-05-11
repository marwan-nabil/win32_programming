#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <ShObjIdl.h>
#include <strsafe.h>
#include <d2d1.h>

#include "graphics.h"
#include "input.h"

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	int Result = 0;

	switch (message)
	{
	case WM_CREATE:
		Result = InitD2DFactoryObject();
		UpdateDPIScale();
		break;
	case WM_PAINT:
		PaintRenderTarget(hWnd);
		break;

	case WM_SIZE:
		ResizeRenderTarget(hWnd);
		return 0;

	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD) wParam);
		return 0;

	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD) wParam);
		return 0;

	case WM_MOUSELEAVE:
		OnMouseLeave(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD) wParam);
		return 0;

	case WM_MOUSEHOVER:
		OnMouseHover(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD) wParam);
		return 0;

	case WM_CLOSE:
		if (MessageBox(hWnd, L"Do you want to quit ?", L"quit prompt", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hWnd);
		}
		else
		{
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return Result;
}


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	/*
	* create a window class object
	*/
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("UserInputApp");
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	/*
	* register the window class object
	*/
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("..."),
			NULL);

		return 1;
	}

	/*
	* create a window
	*/
	static TCHAR szWindowClass[] = _T("UserInputApp");
	static TCHAR szTitle[] = _T("simple user input capturing application");

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("..."),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	/*
	* the message loop
	*/
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}