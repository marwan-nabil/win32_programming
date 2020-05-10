#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <ShObjIdl.h>
#include <strsafe.h>

#define MAX_FILENAME_LENGTH  100

typedef struct WindwoStateTag {
	WCHAR FileName[MAX_FILENAME_LENGTH];
} WindowState;


int OpenFilePicker(WindowState *WState)
{
	// configure COM mode
	HRESULT Result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(Result))
	{
		IFileOpenDialog *FilePickerDialog;
		// create the FilePicker dialog object
		Result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog,
			(void **) &FilePickerDialog);

		if (SUCCEEDED(Result))
		{
			// show the dialog
			Result = FilePickerDialog->Show(NULL);
			if (SUCCEEDED(Result))
			{
				IShellItem *PickedItem;
				Result = FilePickerDialog->GetResult(&PickedItem);
				if (SUCCEEDED(Result))
				{
					LPWSTR FileName;
					Result = PickedItem->GetDisplayName(SIGDN_FILESYSPATH, &FileName);
					if (SUCCEEDED(Result))
					{
						// change the window state
						lstrcpy(WState->FileName, FileName);
						CoTaskMemFree(FileName);
					}
					// release the shell item object
					PickedItem->Release();
				}
			}
			FilePickerDialog->Release();
		}
		CoUninitialize();
	}

	return 0;
}


LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	WindowState *WState;
	WPARAM VKCode = wParam;

	// initialize window state on window creation
	if (message == WM_CREATE)
	{
		CREATESTRUCT *CreateStruct = (CREATESTRUCT *) lParam;
		WState = (WindowState *) CreateStruct->lpCreateParams;
		// save the windowstate struct pointer to this window
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) WState);
	}
	// get current window state on subsequent messages
	else
	{
		LONG_PTR StatePtr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		WState = (WindowState *) StatePtr;
	}

	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hWnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));

		TextOut(hdc, 5, 5, (LPCWSTR) WState->FileName, wcslen((LPCWSTR) WState->FileName));

		EndPaint(hWnd, &ps);
		break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (VKCode == 'O')
		{
			OpenFilePicker(WState);
			// trigger a repaint
			UpdateWindow(hWnd);
		}
		break;

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
	wcex.lpszClassName = _T("DesktopApp");
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
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("simple win32 application");

	// initialize some window-specific state
	WindowState WState = {};
	StringCchCopy((STRSAFE_LPWSTR) &WState.FileName, sizeof(WState.FileName), _T("no file selected"));

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