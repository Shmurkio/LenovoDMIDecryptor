#pragma once

#pragma once

#include <Windows.h>
#include "File.hpp"

namespace Window
{
    struct WINDOW_STATE
    {
        bool Dragging = false;
        POINT DragStartMouse = {};
        POINT DragStartWindow = {};
        File::PFILE_INFO FileInfo = nullptr;
    };

    auto SetFileInfo(WINDOW_STATE& State, File::PFILE_INFO FileInfo) -> void;

    auto HandleUnsavedChanges(HWND hWnd, WINDOW_STATE& State) -> bool;

    auto BeginWindowDrag(HWND hWnd, WINDOW_STATE& State) -> void;
    auto UpdateWindowDrag(HWND hWnd, WINDOW_STATE& State) -> void;
    auto EndWindowDrag(WINDOW_STATE& State) -> void;

    auto WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT;
}