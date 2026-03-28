#include "Window.hpp"

#include <tchar.h>
#include "imgui_impl_win32.h"

namespace Window
{
    static WINDOW_STATE* gWindowState = nullptr;

    auto SetFileInfo(WINDOW_STATE& State, File::PFILE_INFO FileInfo) -> void
    {
        State.FileInfo = FileInfo;
        gWindowState = &State;
    }

    auto HandleUnsavedChanges(HWND hWnd, WINDOW_STATE& State) -> bool
    {
        if (!State.FileInfo)
        {
            return true;
        }

        if (!State.FileInfo->Loaded || !State.FileInfo->Dirty)
        {
            return true;
        }

        const auto Result = MessageBox(
            hWnd,
            _T("You have unsaved changes. Do you want to save them?"),
            _T("Unsaved changes"),
            MB_ICONWARNING | MB_YESNOCANCEL
        );

        if (Result == IDYES)
        {
            if (!File::SaveFile(*State.FileInfo))
            {
                MessageBox(hWnd, _T("Failed to save file."), _T("Error"), MB_ICONERROR);
                return false;
            }

            return true;
        }

        if (Result == IDNO)
        {
            return true;
        }

        return false;
    }

    auto BeginWindowDrag(HWND hWnd, WINDOW_STATE& State) -> void
    {
        State.Dragging = true;

        GetCursorPos(&State.DragStartMouse);

        RECT WindowRect{};
        GetWindowRect(hWnd, &WindowRect);

        State.DragStartWindow.x = WindowRect.left;
        State.DragStartWindow.y = WindowRect.top;
    }

    auto UpdateWindowDrag(HWND hWnd, WINDOW_STATE& State) -> void
    {
        if (!State.Dragging)
        {
            return;
        }

        if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        {
            State.Dragging = false;
            return;
        }

        POINT MousePos = {};
        GetCursorPos(&MousePos);

        const auto Dx = MousePos.x - State.DragStartMouse.x;
        const auto Dy = MousePos.y - State.DragStartMouse.y;

        SetWindowPos(
            hWnd,
            nullptr,
            State.DragStartWindow.x + Dx,
            State.DragStartWindow.y + Dy,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    auto EndWindowDrag(WINDOW_STATE& State) -> void
    {
        State.Dragging = false;
    }

    auto WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
        {
            return true;
        }

        switch (Msg)
        {
            case WM_CLOSE:
            {
                if (!gWindowState || HandleUnsavedChanges(hWnd, *gWindowState))
                {
                    DestroyWindow(hWnd);
                }

                return 0;
            }

            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
        }

        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
}