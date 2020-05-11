#include<Windows.h>
#include <d2d1.h>
#include <strsafe.h>
#include "graphics.h"

D2D1_POINT_2F MousePosition = D2D1::Point2F();
static ID2D1HwndRenderTarget *RenderTarget = NULL;
static ID2D1SolidColorBrush *Brush = NULL;
static ID2D1Factory *D2D_Factory = NULL;
D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(), 0, 0);
static float DPIScaleX = 1.0f;
static float DPIScaleY = 1.0f;


int InitD2DFactoryObject()
{
	if (FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2D_Factory)))
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void UpdateDPIScale()
{
	float dpiX, dpiY;
	D2D_Factory->GetDesktopDpi(&dpiX, &dpiY);
	DPIScaleX = dpiX / 96.0f;
	DPIScaleY = dpiY / 96.0f;
}

D2D1_POINT_2F PixelsToDips(DWORD x, DWORD y)
{
	return D2D1::Point2F((float) x / DPIScaleX, (float) y / DPIScaleY);
}

static void SafeReleaseRenderTarget(ID2D1HwndRenderTarget **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

static void SafeReleaseBrush(ID2D1SolidColorBrush **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

static void CalculateLayout()
{
	//if (RenderTarget != NULL)
	//{
	//	D2D1_SIZE_F size = RenderTarget->GetSize(); // in DIPs
	//	float x = size.width / 2;
	//	float y = size.height / 2;
	//	float radius = min(x, y);
	//	ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	//}
}

void ResizeRenderTarget(HWND Window)
{
	if (RenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(Window, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		RenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(Window, NULL, FALSE);
	}
}

static HRESULT CreateGraphicsResources(HWND Window)
{
	HRESULT Result = S_OK;
	if (RenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(Window, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		Result = D2D_Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(Window, size),
			&RenderTarget);

		if (SUCCEEDED(Result))
		{
			D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			Result = RenderTarget->
				CreateSolidColorBrush(color, &Brush);

			if (SUCCEEDED(Result))
			{
				CalculateLayout();
			}
		}
	}

	return Result;
}


static void DiscardGraphicsResources()
{
	SafeReleaseRenderTarget(&RenderTarget);
	SafeReleaseBrush(&Brush);
}

void DrawClockHand(float HandLength, float Angle, float StrokeWidth)
{
	RenderTarget->SetTransform(
		D2D1::Matrix3x2F::Rotation(Angle, ellipse.point)
		);

	// endPoint defines one end of the hand.
	D2D_POINT_2F endPoint = D2D1::Point2F(
		ellipse.point.x,
		ellipse.point.y - (ellipse.radiusY * HandLength)
		);

	// Draw a line from the center of the ellipse to endPoint.
	RenderTarget->DrawLine(
		ellipse.point, endPoint, Brush, StrokeWidth);
}

void GetTimeParams(float *HourAngle, float *MinuteAngle)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	*HourAngle = (360.0f / 12) * (time.wHour) + (time.wMinute * 0.5f);
	*MinuteAngle = (360.0f / 60) * (time.wMinute);
}

void PaintRenderTarget(HWND Window)
{
	HRESULT Result = CreateGraphicsResources(Window);
	if (SUCCEEDED(Result))
	{
		PAINTSTRUCT Ps;
		BeginPaint(Window, &Ps);

		RenderTarget->BeginDraw();
		RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

		Brush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkOrange));
		RenderTarget->FillEllipse(ellipse, Brush);

		Brush->SetColor(D2D1::ColorF(D2D1::ColorF::Brown));
		RenderTarget->DrawEllipse(ellipse, Brush);

		// draw clock hands
		Brush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGreen));
		// get time parameters to draw clock hands
		float HourAngle, MinuteAngle;
		GetTimeParams(&HourAngle, &MinuteAngle);
		// draw hours hand
		DrawClockHand(0.4f, HourAngle, 3.0f);
		// draw minutes hand
		DrawClockHand(0.8f, MinuteAngle, 3.0f);
		RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		Result = RenderTarget->EndDraw();
		// flush graphics resources (render target and brush) on any failure to draw
		if (FAILED(Result) || Result == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}

		EndPaint(Window, &Ps);
	}
}
