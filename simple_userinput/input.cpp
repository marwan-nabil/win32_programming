#include<Windows.h>
#include "input.h"
#include "graphics.h"

static bool IsMouseTracked = false;
program_mode ProgramMode;

static void EnableOtherMouseMessages(HWND Window);

void OnLButtonDown(HWND Window, int pixelX, int pixelY, DWORD flags)
{
	SetCapture(Window);
	ellipse.point = MousePosition = PixelsToDips(pixelX, pixelY);
	ellipse.radiusX = ellipse.radiusY = 1.0f;
	InvalidateRect(Window, NULL, FALSE); // triggers WM_PAINT
}

void OnLButtonUp()
{
	ReleaseCapture();
}

void OnMouseMove(HWND Window, int pixelX, int pixelY, DWORD flags)
{
	if (!IsMouseTracked)
	{
		EnableOtherMouseMessages(Window);
	}

	D2D1_POINT_2F OldMousePos = MousePosition;
	// check if left MB is still pressed
	if (flags & MK_LBUTTON)
	{
		D2D1_POINT_2F MousePositionInDIPs = PixelsToDips(pixelX, pixelY);
		float width = (MousePositionInDIPs.x - OldMousePos.x) / 2;
		float height = (MousePositionInDIPs.y - OldMousePos.y) / 2;
		float x1 = OldMousePos.x + width;
		float y1 = OldMousePos.y + height;
		ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
		InvalidateRect(Window, NULL, FALSE);
	}
}

void ConfineMouseMovement(HWND Window)
{
	RECT rc;
	GetClientRect(Window, &rc);
	POINT TopLeft = { rc.left, rc.top };
	POINT BottomRight = { rc.right, rc.bottom};
	ClientToScreen(Window, &TopLeft);
	ClientToScreen(Window, &BottomRight);
	SetRect(&rc, TopLeft.x, TopLeft.y, BottomRight.x, BottomRight.y);
	ClipCursor(&rc);
}

static void EnableOtherMouseMessages(HWND Window)
{
	// enable other mouse messages
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = Window;
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&tme);
	IsMouseTracked = true;
}

void OnMouseHover(HWND Window, int pixelX, int pixelY, DWORD flags)
{
	IsMouseTracked = false;
}

void OnMouseLeave(HWND Window, int pixelX, int pixelY, DWORD flags)
{
	OutputDebugStringW(L"WM_MOUSELEAVE message received\n");
	IsMouseTracked = false;
}

void SetMode(program_mode NewProgramMode)
{
	ProgramMode = NewProgramMode;
}