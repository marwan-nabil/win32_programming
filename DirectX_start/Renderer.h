#pragma once

#include <Windows.h>
#include <D3d11.h>
#include <dxgi1_3.h>


void CreateDeviceDependentResources();
void CreateWindowSizeDependentResources();
void Update();
void Render();

void ReleaseResources();