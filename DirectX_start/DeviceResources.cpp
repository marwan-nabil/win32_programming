#include <windows.h>
#include <tchar.h>
#include <D3d11.h>
#include <dxgi1_3.h>

#include "DeviceResources.h"

// Direct3D Device resources
static ID3D11Device *D3D_Device;
static ID3D11DeviceContext *D3D_DeviceContext;
static IDXGISwapChain *SwapChain;

// DXGI swap chain device resources
static ID3D11Texture2D *BackBuffer;
static ID3D11RenderTargetView *RenderTarget;

// Direct3D device resources for the depth stencil
static ID3D11Texture2D *DepthStencil;
static ID3D11DepthStencilView *DepthStencilView;

// Direct3D device / device resource metadata
static D3D_FEATURE_LEVEL FeatureLevel;
static D3D11_TEXTURE2D_DESC BackBufferDescription;
static D3D11_VIEWPORT ViewPort;


ID3D11Device *GetDevice()
{
    return D3D_Device;
};


ID3D11DeviceContext *GetDeviceContext()
{
    return D3D_DeviceContext;
}


ID3D11RenderTargetView *GetRenderTarget()
{
    return RenderTarget;
}


ID3D11DepthStencilView *GetDepthStencil()
{
    return DepthStencilView;
}


// creates D3D_Device and D3D_DeviceContext
HRESULT CreateDeviceResources(HWND Window)
{
#if 0
    HRESULT Result = S_OK;
    // supported feature levels
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_9_1,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1
    };

    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

    Result = D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        deviceFlags,                // Set debug and Direct2D compatibility flags.
        levels,                     // List of feature levels this app can support.
        ARRAYSIZE(levels),          // Size of the list above.
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &D3D_Device,	// Returns the Direct3D device created.
        &FeatureLevel,  // Returns feature level of device created.
        &D3D_DeviceContext	// Returns the device immediate context.
        );

    if(FAILED(Result))
    {
        // Handle device interface creation failure if it occurs.
        // For example, reduce the feature level requirement, or fail over 
        // to WARP rendering.
    }

    return Result;
#else
    HRESULT Result = S_OK;
    // supported feature levels
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_9_1,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1
    };

    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
    desc.Windowed = TRUE;
    desc.BufferCount = 2;
    desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count = 1;      //multisampling setting
    desc.SampleDesc.Quality = 0;    //vendor-specific flag
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.OutputWindow = Window;

    Result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        &desc,
        &SwapChain,
        &D3D_Device,
        &FeatureLevel,
        &D3D_DeviceContext
        );

    // Configure the back buffer and viewport.
    Result = SwapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (void **) &BackBuffer);

    BackBuffer->GetDesc(&BackBufferDescription);

    ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
    ViewPort.Height = (float) BackBufferDescription.Height;
    ViewPort.Width = (float) BackBufferDescription.Width;
    ViewPort.MinDepth = 0;
    ViewPort.MaxDepth = 1;


    D3D_DeviceContext->RSSetViewports(
        1,
        &ViewPort
        );

    Result = D3D_Device->CreateRenderTargetView(
        BackBuffer,
        nullptr,
        &RenderTarget
        );

    return Result;
#endif
}


// creates SwapChain
//HRESULT CreateWindowResources(HWND Window)
//{
//    HRESULT Result = S_OK;
//
//    DXGI_SWAP_CHAIN_DESC SwapChainDescription;
//    ZeroMemory(&SwapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));
//    SwapChainDescription.Windowed = TRUE; // Sets the initial state of full-screen mode.
//    SwapChainDescription.BufferCount = 2;
//    SwapChainDescription.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//    SwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    SwapChainDescription.SampleDesc.Count = 1;      // multisampling setting
//    SwapChainDescription.SampleDesc.Quality = 0;    // vendor-specific flag
//    SwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
//    SwapChainDescription.OutputWindow = Window;
//
//    // Create the DXGI device object to use in other factories, such as Direct2D.
//    IDXGIDevice *Device = (IDXGIDevice *) D3D_Device;
//    // Create swap chain.
//    IDXGIAdapter *Adapter = NULL;
//    IDXGIFactory *Factory = NULL;
//
//    Result = Device->GetAdapter(&Adapter);
//
//    if(SUCCEEDED(Result))
//    {
//        Adapter->GetParent(IID_PPV_ARGS(&Factory));
//
//        Result = Factory->
//            CreateSwapChain(D3D_Device, &SwapChainDescription, &SwapChain);
//    }
//
//    Result = ConfigureBackBuffer();
//    return Result;
//}


// creates BackBuffer, RenderTarget, BackBufferDescription, 
// DepthStencil, DepthStencilView and ViewPort
HRESULT ConfigureBackBuffer()
{
    HRESULT Result = S_OK;

    Result = SwapChain->
        GetBuffer(0, __uuidof(ID3D11Texture2D), (void **) &BackBuffer);

    Result = D3D_Device->
        CreateRenderTargetView(BackBuffer, nullptr,
                               &RenderTarget);

    BackBuffer->GetDesc(&BackBufferDescription);

    // Create a depth-stencil view for use with 3D rendering if needed.
    CD3D11_TEXTURE2D_DESC DepthStencilDescription;

    DepthStencilDescription.Width = (UINT) BackBufferDescription.Width;
    DepthStencilDescription.Height = (UINT) BackBufferDescription.Height;
    DepthStencilDescription.MipLevels = 1;
    DepthStencilDescription.ArraySize = 1;
    DepthStencilDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthStencilDescription.SampleDesc.Count = 1;
    DepthStencilDescription.SampleDesc.Quality = 0;
    DepthStencilDescription.Usage = D3D11_USAGE_DEFAULT;
    DepthStencilDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    DepthStencilDescription.CPUAccessFlags = 0;
    DepthStencilDescription.MiscFlags = 0;

    D3D_Device->CreateTexture2D(&DepthStencilDescription,
                                nullptr,
                                &DepthStencil);

    // TODO: change this to c
    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

    D3D_Device->CreateDepthStencilView(DepthStencil,
                                       &depthStencilViewDesc,
                                       &DepthStencilView);

    ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
    ViewPort.Height = (float) BackBufferDescription.Height;
    ViewPort.Width = (float) BackBufferDescription.Width;
    ViewPort.MinDepth = 0;
    ViewPort.MaxDepth = 1;

    D3D_DeviceContext->RSSetViewports(1, &ViewPort);

    return Result;
}


HRESULT ReleaseBackBuffer()
{
    HRESULT Result = S_OK;

    // Release the render target view based on the back buffer:
    RenderTarget->Release();
    RenderTarget = NULL;

    // Release the back buffer itself:
    BackBuffer->Release();
    BackBuffer = NULL;

    // The depth stencil will need to be resized, so release it (and view):
    DepthStencilView->Release();
    DepthStencilView = NULL;
    DepthStencil->Release();
    DepthStencil = NULL;

    // After releasing references to these resources, we need to call Flush() to 
    // ensure that Direct3D also releases any references it might still have to
    // the same resources - such as pipeline bindings.
    D3D_DeviceContext->Flush();

    return Result;
}


HRESULT GoFullScreen()
{
    HRESULT Result = S_OK;
    Result = SwapChain->SetFullscreenState(TRUE, NULL);


    // Swap chains created with the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL flag need to
    // call ResizeBuffers in order to realize a full-screen mode switch. Otherwise, 
    // your next call to Present will fail.

    // Before calling ResizeBuffers, you have to release all references to the back 
    // buffer device resource.
    ReleaseBackBuffer();

    // Now we can call ResizeBuffers.
    Result = SwapChain->ResizeBuffers(
        0,                   // Number of buffers. Set this to 0 to preserve the existing setting.
        0, 0,                // Width and height of the swap chain. Set to 0 to match the screen resolution.
        DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer format.
        0);

    // Then we can recreate the back buffer, depth buffer, and so on.
    Result = ConfigureBackBuffer();

    return Result;
}


HRESULT GoWindowed()
{
    HRESULT Result = S_OK;

    Result = SwapChain->SetFullscreenState(FALSE, NULL);

    // Swap chains created with the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL flag need to
    // call ResizeBuffers in order to realize a change to windowed mode. Otherwise, 
    // your next call to Present will fail.

    // Before calling ResizeBuffers, you have to release all references to the back 
    // buffer device resource.
    ReleaseBackBuffer();

    // Now we can call ResizeBuffers.
    Result = SwapChain->ResizeBuffers(
        0,                   // Number of buffers. Set this to 0 to preserve the existing setting.
        0, 0,                // Width and height of the swap chain. MUST be set to a non-zero value. For example, match the window size.
        DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer format.
        0
        );

    // Then we can recreate the back buffer, depth buffer, and so on.
    Result = ConfigureBackBuffer();

    return Result;
}


// Returns the aspect ratio of the back buffer.
float GetAspectRatio()
{
    return (float) (BackBufferDescription.Width) / (float) (BackBufferDescription.Height);
}


// Present frame:
// Show the frame on the primary surface.
void Present()
{
    SwapChain->Present(1, 0);
}
