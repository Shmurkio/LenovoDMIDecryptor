#include "D3D.hpp"

namespace D3D
{
    static ID3D11Device* gDevice = nullptr;
    static ID3D11DeviceContext* gDeviceContext = nullptr;
    static IDXGISwapChain* gSwapChain = nullptr;
    static ID3D11RenderTargetView* gRenderTargetView = nullptr;

    auto CreateDevice(HWND hWnd) -> bool
    {
        DXGI_SWAP_CHAIN_DESC Sd = {};
        Sd.BufferCount = 2;
        Sd.BufferDesc.Width = 0;
        Sd.BufferDesc.Height = 0;
        Sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        Sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        Sd.OutputWindow = hWnd;
        Sd.SampleDesc.Count = 1;
        Sd.Windowed = TRUE;
        Sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT CreateDeviceFlags = 0;
        D3D_FEATURE_LEVEL FeatureLevel = {};
        const D3D_FEATURE_LEVEL FeatureLevelArray[] = { D3D_FEATURE_LEVEL_11_0 };

        const auto Hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            CreateDeviceFlags,
            FeatureLevelArray,
            1,
            D3D11_SDK_VERSION,
            &Sd,
            &gSwapChain,
            &gDevice,
            &FeatureLevel,
            &gDeviceContext
        );

        if (FAILED(Hr))
        {
            return false;
        }

        CreateRenderTarget();

        return true;
    }

    auto CleanupDevice() -> void
    {
        CleanupRenderTarget();

        if (gSwapChain)
        {
            gSwapChain->Release();
            gSwapChain = nullptr;
        }

        if (gDeviceContext)
        {
            gDeviceContext->Release();
            gDeviceContext = nullptr;
        }

        if (gDevice)
        {
            gDevice->Release();
            gDevice = nullptr;
        }
    }

    auto CreateRenderTarget() -> void
    {
        if (!gSwapChain || !gDevice)
        {
            return;
        }

        ID3D11Texture2D* BackBuffer = nullptr;

        if (SUCCEEDED(gSwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer))))
        {
            gDevice->CreateRenderTargetView(BackBuffer, nullptr, &gRenderTargetView);
            BackBuffer->Release();
        }
    }

    auto CleanupRenderTarget() -> void
    {
        if (gRenderTargetView)
        {
            gRenderTargetView->Release();
            gRenderTargetView = nullptr;
        }
    }

    auto GetDevice() -> ID3D11Device*
    {
        return gDevice;
    }

    auto GetDeviceContext() -> ID3D11DeviceContext*
    {
        return gDeviceContext;
    }

    auto GetSwapChain() -> IDXGISwapChain*
    {
        return gSwapChain;
    }

    auto GetRenderTargetView() -> ID3D11RenderTargetView*
    {
        return gRenderTargetView;
    }
}