#include "LenovoDMIDecryptor.hpp"
#include "File.hpp"
#include "Cast.hpp"
#include "Util.hpp"
#include "Lenovo.hpp"
#include "Window.hpp"
#include "AppState.hpp"
#include "UI.hpp"
#include "D3D.hpp"

static APP_STATE gAppState;

static auto WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) -> int
{
    Window::SetFileInfo(gAppState.Window, &gAppState.FileInfo);

    WNDCLASSEX Wc =
    {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        Window::WndProc,
        0,
        0,
        hInstance,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        _T("Lenovo DMI Decryptor"),
        nullptr
    };
    RegisterClassEx(&Wc);

    constexpr int WindowWidth = 980;
    constexpr int WindowHeight = 640;

    auto hWnd = CreateWindow(Wc.lpszClassName, _T("Lenovo DMI Decryptor"), WS_POPUP, 100, 100, WindowWidth, WindowHeight, nullptr, nullptr, Wc.hInstance, nullptr);

    if (!D3D::CreateDevice(hWnd))
    {
        return 1;
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& Io = ImGui::GetIO();
    Io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(D3D::GetDevice(), D3D::GetDeviceContext());

    bool Done = false;

    while (!Done)
    {
        MSG Msg;

        while (PeekMessage(&Msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);

            if (Msg.message == WM_QUIT)
            {
                Done = true;
            }
        }

        if (Done)
        {
            break;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        UI::DrawMainWindow(hWnd, gAppState);

        ImGui::Render();
        const float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        auto* RenderTargetView = D3D::GetRenderTargetView();
        D3D::GetDeviceContext()->OMSetRenderTargets(1, &RenderTargetView, nullptr);
        D3D::GetDeviceContext()->ClearRenderTargetView(RenderTargetView, ClearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        D3D::GetSwapChain()->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    D3D::CleanupDevice();
    DestroyWindow(hWnd);
    UnregisterClass(Wc.lpszClassName, Wc.hInstance);

    return 0;
}