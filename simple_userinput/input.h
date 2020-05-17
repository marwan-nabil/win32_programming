#pragma once

// program mode (draw / select)
enum program_mode {
    DRAW_MODE, SELECT_MODE
};

extern program_mode ProgramMode;

void OnLButtonDown(HWND Window, int pixelX, int pixelY, DWORD flags);
void OnLButtonUp();
void OnMouseMove(HWND Window, int pixelX, int pixelY, DWORD flags);
void ConfineMouseMovement(HWND Window);
void OnMouseLeave(HWND Window, int pixelX, int pixelY, DWORD flags);
void OnMouseHover(HWND Window, int pixelX, int pixelY, DWORD flags);

void SetMode(program_mode NewProgramMode);