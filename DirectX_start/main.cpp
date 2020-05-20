#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "DeviceResources.h"
#include "Renderer.h"

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
    HRESULT Result = S_OK;
    // --------------------------------------------
    // create a window class object and register it
    // --------------------------------------------
    WNDCLASSEX WindowClass;
    TCHAR WindowClassName[] = _T("Direct3DAPP");
    TCHAR WindowTitle[] = _T("a simple DirectX application");
    HMENU Menu = NULL;

    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = WindowClassName;
    WindowClass.hIconSm = LoadIcon(WindowClass.hInstance, IDI_APPLICATION);

    if(!RegisterClassEx(&WindowClass))
    {
        return 1;
    }

    // ---------------
    // create a window
    // ---------------
    int Width = 640;
	int Height = 480;

    HWND hWnd = CreateWindow(WindowClassName, WindowTitle, WS_OVERLAPPEDWINDOW, 
        			 		 CW_USEDEFAULT, CW_USEDEFAULT,
                             Width, Height,
                     		 NULL, Menu, hInstance, NULL);
    if(!hWnd)
    {
        return 1;
    }
    
    // -------------------------
    // initialize Direct3D stuff
    // -------------------------

    // Instantiate the device manager
    CreateDeviceResources(hWnd);
    // Instantiate the renderer.
    CreateDeviceDependentResources();
    // We have a window, so initialize window size-dependent resources.
    //CreateWindowResources(hWnd);
    CreateWindowSizeDependentResources();


    // show the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // ----------------
    // the message loop
    // ----------------
    Running = true;
    MSG msg;
    while (Running)
    {
        // BUG: message saturation happens
        // TODO: fix the message saturation problem
	    if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
	    {
	        TranslateMessage(&msg);
	        DispatchMessage(&msg);
	    }
	    else
	    {
	        RendererUpdate();

	        RendererRender();

            Present();
	    }
    }
 
    return Result;
}