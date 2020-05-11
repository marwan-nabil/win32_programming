#pragma once
#include <d2d1.h>

extern D2D1_POINT_2F MousePosition;
extern D2D1_ELLIPSE ellipse;

int InitD2DFactoryObject();
void PaintRenderTarget(HWND Window);
void ResizeRenderTarget(HWND Window);
void UpdateDPIScale();
D2D1_POINT_2F PixelsToDips(DWORD x, DWORD y);