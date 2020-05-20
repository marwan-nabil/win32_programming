#pragma once

#include <Windows.h>
#include <D3d11.h>
#include <dxgi1_3.h>

HRESULT CreateDeviceResources(HWND hWnd);
//HRESULT CreateWindowResources(HWND hWnd);

HRESULT ConfigureBackBuffer();
HRESULT ReleaseBackBuffer();
HRESULT GoFullScreen();
HRESULT GoWindowed();

float GetAspectRatio();

ID3D11Device *GetDevice();
ID3D11DeviceContext *GetDeviceContext();
ID3D11RenderTargetView *GetRenderTarget();
ID3D11DepthStencilView *GetDepthStencil();

void Present();