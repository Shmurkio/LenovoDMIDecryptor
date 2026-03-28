#pragma once

#include <Windows.h>
#include "AppState.hpp"

namespace UI
{
    auto DrawMainWindow(HWND hWnd, APP_STATE& State) -> void;

    auto DrawTitleBar(HWND hWnd, APP_STATE& State) -> void;
    auto DrawMenuBar(HWND hWnd, APP_STATE& State) -> void;
    auto DrawDmiPanel(APP_STATE& State) -> void;
    auto DrawFileInfoPanel(APP_STATE& State) -> void;
    auto DrawEncryptionPanel(HWND hWnd, APP_STATE& State) -> void;

    auto OpenImageFile(HWND hWnd, APP_STATE& State) -> bool;
    auto SaveImageFile(HWND hWnd, APP_STATE& State) -> bool;
}