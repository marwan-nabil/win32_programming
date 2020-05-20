#pragma once

#include <Windows.h>
#include <D3d11.h>
#include <dxgi1_3.h>
#include <directxmath.h>
#include <stdio.h>

#include "Renderer.h"
#include "DeviceResources.h"

////////////////////////////////
// types
////////////////////////////////

// Variables for rendering the cube
typedef struct {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
} constant_buffer;

// Per-vertex data
typedef struct {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 color;
} vertex_position_color;

typedef struct {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
} vertex_position_color_tangent;

////////////////////////////////
// Globals
////////////////////////////////

static constant_buffer ConstantBufferData;
static unsigned int  IndexCount;
static unsigned int  FrameCount = 0;

// Direct3D device resources
//ID3DXEffect* m_pEffect;
static ID3D11Buffer *VertexBuffer;
static ID3D11Buffer *IndexBuffer;
static ID3D11VertexShader *VertexShader;
static ID3D11InputLayout *InputLayout;
static ID3D11InputLayout *InputLayoutExtended;
static ID3D11PixelShader *PixelShader;
static ID3D11Buffer *ConstantBuffer;

//
// Prototypes
//
HRESULT CreateShaders();
HRESULT CreateCube();
void    CreateViewAndPerspective();


// Create Direct3D shader resources by loading the .cso files.
HRESULT CreateShaders()
{
    HRESULT Result = S_OK;

    // Use the Direct3D device to load resources into graphics memory.
    ID3D11Device *Device = GetDevice();

    // You'll need to use a file loader to load the shader bytecode. In this
    // example, we just use the standard library.
    FILE *VertexShaderFile, *PixelShaderFile;
    BYTE *Bytes;

    size_t DestSize = 4096;
    size_t BytesRead = 0;
    Bytes = new BYTE[DestSize];

    fopen_s(&VertexShaderFile, "CubeVertexShader.cso", "rb");
    BytesRead = fread_s(Bytes, DestSize, 1, 4096, VertexShaderFile);

    Result = Device->CreateVertexShader(Bytes, BytesRead, nullptr, &VertexShader);

    D3D11_INPUT_ELEMENT_DESC InputDescriptionsArray[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    Result = Device->CreateInputLayout(InputDescriptionsArray,
                                       ARRAYSIZE(InputDescriptionsArray),
                                       Bytes, BytesRead,
                                       &InputLayout);
    delete Bytes;

    Bytes = new BYTE[DestSize];
    BytesRead = 0;
    fopen_s(&PixelShaderFile, "CubePixelShader.cso", "rb");
    BytesRead = fread_s(Bytes, DestSize, 1, 4096, PixelShaderFile);
    Result = Device->CreatePixelShader(Bytes, BytesRead, nullptr, &PixelShader);

    delete Bytes;

    CD3D11_BUFFER_DESC ConstantBufferDescription(
        sizeof(constant_buffer),
        D3D11_BIND_CONSTANT_BUFFER);

    Result = Device->CreateBuffer(&ConstantBufferDescription,
                                  nullptr, &ConstantBuffer);

    fclose(VertexShaderFile);
    fclose(PixelShaderFile);

    /* (other option)
    // Load the extended shaders with lighting calculations.
    Bytes = new BYTE[DestSize];
    BytesRead = 0;
    fopen_s(&VertexShaderFile, "CubeVertexShaderLighting.cso", "rb");
    BytesRead = fread_s(Bytes, DestSize, 1, 4096, VertexShaderFile);
    Result = Device->CreateVertexShader(Bytes, BytesRead, nullptr, &VertexShader);

    D3D11_INPUT_ELEMENT_DESC InputDescriptionsArray[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    Result = Device->CreateInputLayout(InputDescriptionsArray,
                                       ARRAYSIZE(InputDescriptionsArray),
                                       Bytes, BytesRead,
                                       &InputLayoutExtended);

    delete Bytes;


    Bytes = new BYTE[DestSize];
    BytesRead = 0;
    fopen_s(&PixelShaderFile, "CubePixelShaderPhongLighting.cso", "rb");
    BytesRead = fread_s(Bytes, DestSize, 1, 4096, PixelShaderFile);
    Result = Device->CreatePixelShader(Bytes, BytesRead, nullptr, &PixelShader);

    delete Bytes;

    fclose(VertexShaderFile);
    fclose(PixelShaderFile);


    Bytes = new BYTE[DestSize];
    BytesRead = 0;
    fopen_s(&PixelShaderFile, "CubePixelShaderTexelLighting.cso", "rb");
    BytesRead = fread_s(Bytes, DestSize, 1, 4096, PixelShaderFile);
    Result = Device->CreatePixelShader(Bytes, BytesRead, nullptr, &PixelShader);

    delete Bytes;

    fclose(PixelShaderFile);
    */
    return Result;
}


// Creates the vertex buffer and index buffer.
HRESULT CreateCube()
{
    HRESULT Result = S_OK;

    // Use the Direct3D device to load resources into graphics memory.
    ID3D11Device *Device = GetDevice();

    // Create cube geometry.
    vertex_position_color CubeVertices[] =
    {
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, 0), },
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), },
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 1, 0), },
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 1, 1), },

        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(1, 0, 0), },
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(1, 0, 1), },
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(1, 1, 0), },
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1, 1, 1), },
    };

    // Create vertex buffer:

    CD3D11_BUFFER_DESC VertexBufferDescription(sizeof(CubeVertices),
                                               D3D11_BIND_VERTEX_BUFFER);

    D3D11_SUBRESOURCE_DATA VertexData;
    ZeroMemory(&VertexData, sizeof(D3D11_SUBRESOURCE_DATA));
    VertexData.pSysMem = CubeVertices;
    VertexData.SysMemPitch = 0;
    VertexData.SysMemSlicePitch = 0;

    Result = Device->CreateBuffer(&VertexBufferDescription, &VertexData, &VertexBuffer);

    // Create index buffer:
    unsigned short CubeIndices[] =
    {
        0, 2, 1, // -x
        1, 2, 3,

        4, 5, 6, // +x
        5, 7, 6,

        0, 1, 5, // -y
        0, 5, 4,

        2, 6, 7, // +y
        2, 7, 3,

        0, 4, 6, // -z
        0, 6, 2,

        1, 3, 7, // +z
        1, 7, 5,
    };

    IndexCount = ARRAYSIZE(CubeIndices);

    CD3D11_BUFFER_DESC IndexBufferDescription(sizeof(CubeIndices), D3D11_BIND_INDEX_BUFFER);

    D3D11_SUBRESOURCE_DATA IndexData;
    ZeroMemory(&IndexData, sizeof(D3D11_SUBRESOURCE_DATA));
    IndexData.pSysMem = CubeIndices;
    IndexData.SysMemPitch = 0;
    IndexData.SysMemSlicePitch = 0;

    Result = Device->CreateBuffer(&IndexBufferDescription, &IndexData, &IndexBuffer);

    return Result;
}


// Create the view matrix and create the perspective matrix.
void CreateViewAndPerspective()
{
    // Use DirectXMath to create view and perspective matrices.
    DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
    DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

    DirectX::XMStoreFloat4x4(&ConstantBufferData.view, 
                             DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up)));

    float AspectRatio = GetAspectRatio();

    DirectX::XMStoreFloat4x4(
        &ConstantBufferData.projection,
        DirectX::XMMatrixTranspose(
            DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(70), AspectRatio, 0.01f, 100.0f)
            )
        );
}


// Create device-dependent resources for rendering.
void CreateDeviceDependentResources()
{
    // Compile shaders using the Effects library.
    CreateShaders();

    // Load the geometry for the spinning cube.
    CreateCube();
}


void CreateWindowSizeDependentResources()
{
    // Create the view matrix and the perspective matrix.
    CreateViewAndPerspective();
}


// RendererUpdate the scene.
void RendererUpdate()
{
    // Rotate the cube 1 degree per frame.
    DirectX::XMStoreFloat4x4(
        &ConstantBufferData.world,
        DirectX::XMMatrixTranspose(
            DirectX::XMMatrixRotationY(
                DirectX::XMConvertToRadians((float) FrameCount++)
                )
            )
        );

    if(FrameCount == MAXUINT)  FrameCount = 0;
}


// RendererRender the cube.
void RendererRender()
{
    // Use the Direct3D device Context to draw.
    ID3D11DeviceContext *Context = GetDeviceContext();

    ID3D11RenderTargetView *RenderTarget = GetRenderTarget();
    ID3D11DepthStencilView *DepthStencil = GetDepthStencil();

    Context->UpdateSubresource(ConstantBuffer, 0, nullptr, 
                               &ConstantBufferData, 0, 0);

    // Clear the render target and the z-buffer.
    const float teal[] = {0.098f, 0.439f, 0.439f, 1.000f};
    Context->ClearRenderTargetView(
        RenderTarget,
        teal
        );
    Context->ClearDepthStencilView(
        DepthStencil,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);

    // Set the render target.
    Context->OMSetRenderTargets(
        1,
        &RenderTarget,
        DepthStencil
        );

    // Set up the IA stage by setting the input topology and layout.
    UINT stride = sizeof(vertex_position_color);
    UINT offset = 0;

    Context->IASetVertexBuffers(
        0,
        1,
        &VertexBuffer,
        &stride,
        &offset
        );

    Context->IASetIndexBuffer(
        IndexBuffer,
        DXGI_FORMAT_R16_UINT,
        0
        );

    Context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );

    Context->IASetInputLayout(InputLayout);

    // Set up the vertex shader stage.
    Context->VSSetShader(
        VertexShader,
        nullptr,
        0
        );

    Context->VSSetConstantBuffers(
        0,
        1,
        &ConstantBuffer
        );

    // Set up the pixel shader stage.
    Context->PSSetShader(
        PixelShader,
        nullptr,
        0
        );

    // Calling Draw tells Direct3D to start sending commands to the graphics device.
    Context->DrawIndexed(
        IndexCount,
        0,
        0
        );
}


// clean up resources
void ReleaseResources()
{
    VertexBuffer->Release();
    IndexBuffer->Release();
    VertexShader->Release();
    InputLayout->Release();
    InputLayoutExtended->Release();
    PixelShader->Release();
    ConstantBuffer->Release();

    VertexBuffer = NULL;
    IndexBuffer = NULL;
    VertexShader = NULL;
    InputLayout = NULL;
    InputLayoutExtended = NULL;
    PixelShader = NULL;
    ConstantBuffer = NULL;
}