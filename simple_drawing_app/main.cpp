#include <ShObjIdl.h>
#include <d2d1.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "resource.h"

// please link me with d2d1.lib
#pragma comment(lib, "d2d1")

/******************************************************************************/
/****************************** Global variables ******************************/
enum program_mode {
    DRAW_MODE, SELECT_MODE, DRAG_MODE
};
program_mode ProgramMode = DRAW_MODE;

struct colored_ellipse {
    D2D1_ELLIPSE ellipse;
    D2D1_COLOR_F color;
} ellipses_array[10];

int SelectedEllipseIdx = 0;

float DPIScaleX = 1.0f;
float DPIScaleY = 1.0f;

struct {
    ID2D1HwndRenderTarget *RenderTarget;
    ID2D1SolidColorBrush *Brush;
    ID2D1Factory *D2D_Factory;
} RenderResources = {NULL, NULL, NULL};

D2D1::ColorF::Enum AvailableColors[] = {
    D2D1::ColorF::Yellow, D2D1::ColorF::Salmon, D2D1::ColorF::LimeGreen
};

size_t nextColorIdx = 0;

HCURSOR CurrentCursor;
D2D1_POINT_2F LastMousePosition = D2D1::Point2F();


/****************************** Global variables ******************************/
/******************************************************************************/

HRESULT CreateGraphicsResources(HWND Window)
{
    HRESULT Result = S_OK;
    if(RenderResources.RenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(Window, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        Result = RenderResources.D2D_Factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(Window, size),
            &RenderResources.RenderTarget);

        if(SUCCEEDED(Result))
        {
            // initial brush color
            const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
            Result = RenderResources.RenderTarget->CreateSolidColorBrush(color, &RenderResources.Brush);
        }
    }
    return Result;
}

void SafeReleaseRenderTarget(ID2D1HwndRenderTarget **ppT)
{
    if(*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

void SafeReleaseBrush(ID2D1SolidColorBrush **ppT)
{
    if(*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

void SafeReleaseD2DFactory(ID2D1Factory **ppT)
{
    if(*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

void DiscardGraphicsResources()
{
    SafeReleaseRenderTarget(&RenderResources.RenderTarget);
    SafeReleaseBrush(&RenderResources.Brush);
}

struct colored_ellipse *GetSelectedEllipse()
{
    if(SelectedEllipseIdx == -1)
    {
        return NULL;
    }
    else
    {
        return &ellipses_array[SelectedEllipseIdx];
    }
}

void DeleteSelectedEllipse()
{
    colored_ellipse *SelectedEllipse = GetSelectedEllipse();
    ellipses_array[SelectedEllipseIdx] = {};
}

void MoveSelectedEllipse(HWND Window, float X, float Y)
{
    colored_ellipse *SelectedEllipse = GetSelectedEllipse();
    if((ProgramMode == SELECT_MODE) && SelectedEllipse)
    {
        SelectedEllipse->ellipse.point.x += X;
        SelectedEllipse->ellipse.point.y += Y;
        InvalidateRect(Window, NULL, FALSE);
    }
}

void UpdateDPIScale()
{
    float dpiX, dpiY;
    RenderResources.D2D_Factory->GetDesktopDpi(&dpiX, &dpiY);
    DPIScaleX = dpiX / 96.0f;
    DPIScaleY = dpiY / 96.0f;
}

D2D1_POINT_2F PixelsToDips(DWORD x, DWORD y)
{
    return D2D1::Point2F((float) x / DPIScaleX, (float) y / DPIScaleY);
}

void ClearSelectedEllipse()
{
    SelectedEllipseIdx = -1;
}

int InitD2DFactoryObject()
{
    if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                &RenderResources.D2D_Factory)))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void DrawColoredEllipse(struct colored_ellipse *Ellipse)
{
    if(Ellipse)
    {
        ID2D1SolidColorBrush *Brush = RenderResources.Brush;
        ID2D1HwndRenderTarget *RenderTarget = RenderResources.RenderTarget;

        Brush->SetColor(Ellipse->color);
        RenderTarget->FillEllipse(Ellipse->ellipse, Brush);
        Brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));  // ellipse border in black
        RenderTarget->DrawEllipse(Ellipse->ellipse, Brush, 1.0f);
    }
}

// given point (x, y) inside the ellipse or not
bool HitTest(struct colored_ellipse *Ellipse, float x, float y)
{
    const float a = Ellipse->ellipse.radiusX;
    const float b = Ellipse->ellipse.radiusY;
    const float x1 = x - Ellipse->ellipse.point.x;
    const float y1 = y - Ellipse->ellipse.point.y;
    const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b * b));
    return d <= 1.0f;
}

void SetProgramMode(program_mode NewProgramMode)
{
    ProgramMode = NewProgramMode;

    LPWSTR cursor;
    switch(ProgramMode)
    {
        case DRAW_MODE:
            cursor = IDC_CROSS;
            break;

        case SELECT_MODE:
            cursor = IDC_HAND;
            break;

        case DRAG_MODE:
            cursor = IDC_SIZEALL;
            break;
    }

    CurrentCursor = LoadCursor(NULL, cursor);
    SetCursor(CurrentCursor);
}

HRESULT InsertEllipse(float X, float Y)
{
    // get a free slot in the ellipses array
    int FreeEllipseIdx = 0;
    for(; FreeEllipseIdx < ARRAYSIZE(ellipses_array); FreeEllipseIdx++)
    {
        if(ellipses_array[FreeEllipseIdx].ellipse.radiusX == 0)
        {
            break;
        }
    }

    if(FreeEllipseIdx == ARRAYSIZE(ellipses_array))
    {
        // array is full
        SelectedEllipseIdx = -1;
        return E_FAIL;
    }
    else
    {
        SelectedEllipseIdx = FreeEllipseIdx;
        colored_ellipse *SelectedEllipse = &ellipses_array[FreeEllipseIdx];
        SelectedEllipse->ellipse.point = D2D1::Point2F(X, Y);
        SelectedEllipse->ellipse.radiusX = SelectedEllipse->ellipse.radiusY = 2.0f;
        SelectedEllipse->color = D2D1::ColorF(AvailableColors[nextColorIdx]);
        nextColorIdx = (nextColorIdx + 1) % ARRAYSIZE(AvailableColors);
        return S_OK;
    }
}

void ResizeRenderTarget(HWND Window)
{
    if(RenderResources.RenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(Window, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        RenderResources.RenderTarget->Resize(size);

        InvalidateRect(Window, NULL, FALSE);
    }
}

void OnPaintHandler(HWND Window)
{
    HRESULT Result = CreateGraphicsResources(Window);
    if(SUCCEEDED(Result))
    {
        PAINTSTRUCT ps;
        BeginPaint(Window, &ps);

        RenderResources.RenderTarget->BeginDraw();

        RenderResources.RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

        // draw all ellipses with black border
        for(int i = 0; i < ARRAYSIZE(ellipses_array); ++i)
        {
            DrawColoredEllipse(&ellipses_array[i]);
        }

        // redraw selected ellipse with red border
        if(colored_ellipse *Ellipse = GetSelectedEllipse())
        {
            RenderResources.Brush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
            RenderResources.RenderTarget->DrawEllipse(Ellipse->ellipse, RenderResources.Brush, 2.0f);
        }

        Result = RenderResources.RenderTarget->EndDraw();

        // on any draw failure, discard graphics resources, to be
        // re-initialized on next paint message
        if(FAILED(Result) || Result == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(Window, &ps);
    }
}

bool HitTestAllEllipses(float X, float Y)
{
    for(int i = 0; i < ARRAYSIZE(ellipses_array); ++i)
    {
        if(HitTest(&ellipses_array[i], X, Y))
        {
            SelectedEllipseIdx = i;
            return TRUE;
        }
    }
    return FALSE;
}

void OnLMouseButtonDownHandler(HWND Window, int PixelX, int PixelY, DWORD flags)
{
    D2D1_POINT_2F DIPPoint = PixelsToDips(PixelX, PixelY);

    if(ProgramMode == DRAW_MODE)
    {
        POINT pt = {PixelX, PixelY};

        if(DragDetect(Window, pt))
        {
            SetCapture(Window);

            // Start a new ellipse.
            LastMousePosition = DIPPoint;
            InsertEllipse(DIPPoint.x, DIPPoint.y);
        }
    }
    else
    {
        ClearSelectedEllipse();

        if(HitTestAllEllipses(DIPPoint.x, DIPPoint.y))
        {
            SetCapture(Window);

            LastMousePosition = GetSelectedEllipse()->ellipse.point;
            LastMousePosition.x -= DIPPoint.x;
            LastMousePosition.y -= DIPPoint.y;

            SetProgramMode(DRAG_MODE);
        }
    }
    InvalidateRect(Window, NULL, FALSE);
}

void OnLMouseButtonUpHandler(HWND Window)
{
    if((ProgramMode == DRAW_MODE) && GetSelectedEllipse())
    {
        ClearSelectedEllipse();
        InvalidateRect(Window, NULL, FALSE);
    }
    else if(ProgramMode == DRAG_MODE)
    {
        SetProgramMode(SELECT_MODE);
    }
    ReleaseCapture();
}

void OnMouseMoveHandler(HWND Window, int PixelX, int PixelY, DWORD flags)
{
    D2D1_POINT_2F DIPPoint = PixelsToDips(PixelX, PixelY);

    colored_ellipse *SelectedEllipse = GetSelectedEllipse();
    if((flags & MK_LBUTTON) && SelectedEllipse)
    {
        if(ProgramMode == DRAW_MODE)
        {
            // Resize the ellipse.
            const float RadiusX = (DIPPoint.x - LastMousePosition.x) / 2;
            const float RadiusY = (DIPPoint.y - LastMousePosition.y) / 2;
            const float CenterPointX = LastMousePosition.x + RadiusX;
            const float CenterPointY = LastMousePosition.y + RadiusY;

            SelectedEllipse->ellipse = D2D1::Ellipse(D2D1::Point2F(CenterPointX, CenterPointY), RadiusX, RadiusY);
        }
        else if(ProgramMode == DRAG_MODE)
        {
            // Move the ellipse.
            SelectedEllipse->ellipse.point.x = DIPPoint.x + LastMousePosition.x;
            SelectedEllipse->ellipse.point.y = DIPPoint.y + LastMousePosition.y;
        }
        InvalidateRect(Window, NULL, FALSE);
    }
}

void OnKeyboardKeyDownHandler(HWND Window, UINT VKCode)
{
    colored_ellipse *SelectedEllipse = GetSelectedEllipse();
    switch(VKCode)
    {
        case VK_BACK:
        case VK_DELETE:
            if((ProgramMode == SELECT_MODE) && SelectedEllipse)
            {
                DeleteSelectedEllipse();
                ClearSelectedEllipse();
                SetProgramMode(SELECT_MODE);
                InvalidateRect(Window, NULL, FALSE);
            };
            break;

        case VK_LEFT:
            MoveSelectedEllipse(Window, -1, 0);
            break;

        case VK_RIGHT:
            MoveSelectedEllipse(Window, 1, 0);
            break;

        case VK_UP:
            MoveSelectedEllipse(Window, 0, -1);
            break;

        case VK_DOWN:
            MoveSelectedEllipse(Window, 0, 1);
            break;
    }
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam,
                         _In_ LPARAM lParam)
{
    int Result = 0;

    switch(message)
    {
        case WM_CREATE:
            if(FAILED(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &RenderResources.D2D_Factory)))
            {
                return -1;  // Fail CreateWindowEx.
            }
            UpdateDPIScale();
            SetProgramMode(DRAW_MODE);
            return 0;

        case WM_PAINT:
            OnPaintHandler(hWnd);
            break;

        case WM_SIZE:
            ResizeRenderTarget(hWnd);
            return 0;

        case WM_LBUTTONDOWN:
            OnLMouseButtonDownHandler(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD) wParam);
            return 0;

        case WM_LBUTTONUP:
            OnLMouseButtonUpHandler(hWnd);
            return 0;

        case WM_MOUSEMOVE:
            OnMouseMoveHandler(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD) wParam);
            return 0;

        case WM_KEYDOWN:
            OnKeyboardKeyDownHandler(hWnd, (UINT) wParam);
            return 0;

        case WM_SETCURSOR:
            if(LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(CurrentCursor);
                return TRUE;
            }
            break;

        case WM_CLOSE:
            if(MessageBox(hWnd, L"Do you want to quit ?", L"quit prompt",
                          MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(hWnd);
            }
            else
            {
                return 0;
            }
            break;

        case WM_DESTROY:
            DiscardGraphicsResources();
            SafeReleaseD2DFactory(&RenderResources.D2D_Factory);
            PostQuitMessage(0);
            return 0;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_DRAW_MODE:
                    SetProgramMode(DRAW_MODE);
                    break;

                case ID_SELECT_MODE:
                    SetProgramMode(SELECT_MODE);
                    break;

                case ID_TOGGLE_MODE:
                    if(ProgramMode == DRAW_MODE)
                    {
                        SetProgramMode(SELECT_MODE);
                    }
                    else
                    {
                        SetProgramMode(DRAW_MODE);
                    }
                    break;
            }
            return 0;

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
    TCHAR szWindowClass[] = _T("SimpleDrawApp");
    TCHAR szTitle[] = _T("a simple ellipse drawing application");

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
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    /*
     * register the window class object
     */
    if(!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("..."), NULL);

        return 1;
    }

    /*
     * create a window
     */
    HWND hWnd =
        CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                     CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);

    if(!hWnd)
    {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("..."), NULL);

        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // register the accelerator table
    HACCEL hAccel = LoadAccelerators(hInstance, 
                                     MAKEINTRESOURCE(IDR_ACCELERATOR_TABLE));

    /*
     * the message loop
     */
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        // intercept accelerator commands
        if(!TranslateAccelerator(hWnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}