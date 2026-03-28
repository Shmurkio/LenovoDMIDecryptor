#pragma once

#include <Windows.h>
#include <d3d11.h>

namespace D3D
{
    auto CreateDevice(HWND hWnd) -> bool;
    auto CleanupDevice() -> void;

    auto CreateRenderTarget() -> void;
    auto CleanupRenderTarget() -> void;

    auto GetDevice() -> ID3D11Device*;
    auto GetDeviceContext() -> ID3D11DeviceContext*;
    auto GetSwapChain() -> IDXGISwapChain*;
    auto GetRenderTargetView() -> ID3D11RenderTargetView*;
}