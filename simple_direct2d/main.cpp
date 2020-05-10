#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <ShObjIdl.h>
#include <strsafe.h>
#include <d2d1.h>


typedef struct {
	ID2D1HwndRenderTarget *RenderTarget;
	ID2D1SolidColorBrush *Brush;
} d2d_resources;


typedef struct WindowStateTag {
	ID2D1Factory *D2D_Factory;
	D2D1_ELLIPSE ellipse;
	d2d_resources *Resources;
	float DPIScaleX;
	float DPIScaleY;
} window_state;


void InitializeDPIScale(window_state *WState)
{
	float dpiX, dpiY;

	WState->D2D_Factory->GetDesktopDpi(&dpiX, &dpiY);

	WState->DPIScaleX = dpiX / 96.0f;
	WState->DPIScaleY = dpiY / 96.0f;
}


float PixelsToDips(DWORD x, float scale)
{
	return (float) x / scale;
}


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

void CalculateLayout(window_state *WState)
{
	if (WState->Resources->RenderTarget != NULL)
	{
		D2D1_SIZE_F size = WState->Resources->RenderTarget->GetSize();
		float x = size.width / 2;
		float y = size.height / 2;
		float radius = min(x, y);
		WState->ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

void ResizeRenderTarget(HWND Window, window_state *WState)
{
	if (WState->Resources->RenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(Window, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		WState->Resources->RenderTarget->Resize(size);
		CalculateLayout(WState);
		InvalidateRect(Window, NULL, FALSE);
	}
}


HRESULT CreateGraphicsResources(HWND Window, window_state *WState)
{
	HRESULT Result = S_OK;
	if (WState->Resources->RenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(Window, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		Result = WState->D2D_Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(Window, size),
			&WState->Resources->RenderTarget);

		if (SUCCEEDED(Result))
		{
			D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			Result = WState->Resources->RenderTarget->
				CreateSolidColorBrush(color, &WState->Resources->Brush);

			if (SUCCEEDED(Result))
			{
				CalculateLayout(WState);
			}
		}
	}

	return Result;
}


void DiscardGraphicsResources(window_state *WState)
{
	SafeRelease(&WState->Resources->RenderTarget);
	SafeRelease(&WState->Resources->Brush);
}

void handle_paint_message(HWND Window, window_state *WState)
{
	HRESULT Result = CreateGraphicsResources(Window, WState);
	if (SUCCEEDED(Result))
	{
		PAINTSTRUCT Ps;
		BeginPaint(Window, &Ps);

		WState->Resources->RenderTarget->BeginDraw();
		WState->Resources->RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		WState->Resources->RenderTarget->FillEllipse(WState->ellipse, WState->Resources->Brush);

		Result = WState->Resources->RenderTarget->EndDraw();
		if (FAILED(Result) || Result == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources(WState);
		}

		EndPaint(Window, &Ps);
	}
}

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	window_state *WState = NULL;

	// initialize window state on window creation
	if (message == WM_CREATE)
	{
		CREATESTRUCT *CreateStruct = (CREATESTRUCT *) lParam;
		WState = (window_state *) CreateStruct->lpCreateParams;
		// save the windowstate struct pointer to this window
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) WState);

		// initialize direct2D factory object
		WState->D2D_Factory = NULL;
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &WState->D2D_Factory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
	}
	// get current window state on subsequent messages
	else
	{
		LONG_PTR StatePtr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		WState = (window_state *) StatePtr;
	}

	switch (message)
	{
	case WM_PAINT:
		handle_paint_message(hWnd, WState);
		break;

	case WM_SIZE:
		ResizeRenderTarget(hWnd, WState);
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

	return 0;
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
	wcex.lpszClassName = _T("Direct2dApp");
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
	static TCHAR szWindowClass[] = _T("Direct2dApp");
	static TCHAR szTitle[] = _T("simple direct2D application");

	d2d_resources Resources = {};
	window_state WState = {};
	WState.Resources = &Resources;
	WState.DPIScaleX = 1.0f;
	WState.DPIScaleY = 1.0f;

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
		NULL,
		NULL,
		hInstance,
		&WState
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