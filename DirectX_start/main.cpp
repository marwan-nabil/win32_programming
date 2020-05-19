#include <windows.h>
#include <tchar.h>

#include "DeviceResources.h"
#include "Renderer.h"

#define MAX_PATH 50

bool Running = false;

LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam,
                         	_In_ LPARAM lParam)
{
    int Result = 0;

    switch(message)
    {
        case WM_CREATE:
            break;

        case WM_PAINT:
            break;

        case WM_SIZE:
            break;

		case WM_QUIT:
			Running = false;
			break;

		case WM_CLOSE:
		{
		    HMENU Menu;
		    Menu = GetMenu(hWnd);
		    if (Menu != NULL)
		    {
		        DestroyMenu(Menu);
		    }
		    DestroyWindow(hWnd);
		    return 0;
		}

		case WM_DESTROY:
		    PostQuitMessage(0);
		    break;

		default:
		    return DefWindowProc(hWnd, message, wParam, lParam);
		    break;
    }

    return Result;
}


int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    /*
     * create a window class object
     */
    WNDCLASSEX wcex;
    TCHAR szWindowClass[] = _T("DXAPP");
    TCHAR szTitle[] = _T("a simple DirectX application");
    HMENU Menu = NULL;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	

	WCHAR szExePath[MAX_PATH];
	GetModuleFileName(NULL, szExePath, MAX_PATH);
    
    /*
     * register the window class object
     */
    if(!RegisterClassEx(&wcex))
    {
        return 1;
    }

    /*
     * create a window
     */
    RECT Rc;
    int Width = 640;
	int Height = 480;
	SetRect(&Rc, 0, 0, Width, Height);        
	AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, (Menu != NULL) ? true : false);

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 
        			 		 CW_USEDEFAULT, CW_USEDEFAULT,
        			 		 (Rc.right - Rc.left), (Rc.bottom - Rc.top),
                     		 NULL, Menu, hInstance, NULL);
    if(!hWnd)
    {
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    /*
     * the message loop
     */
    Running = true;
    MSG msg;
    while (Running)
    {
	    if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	    {
	        TranslateMessage(&msg);
	        DispatchMessage(&msg);
	    }
	    else
	    {
	        Update();

	        Render();

            Present();
	    }
    }
 
    return (int) msg.wParam;
}