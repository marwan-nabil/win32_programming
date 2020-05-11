#pragma once

void OnLButtonDown(HWND Window, int pixelX, int pixelY, DWORD flags);
void OnLButtonUp();
void OnMouseMove(HWND Window, int pixelX, int pixelY, DWORD flags);
void ConfineMouseMovement(HWND Window);
void OnMouseLeave(HWND Window, int pixelX, int pixelY, DWORD flags);
void OnMouseHover(HWND Window, int pixelX, int pixelY, DWORD flags);
