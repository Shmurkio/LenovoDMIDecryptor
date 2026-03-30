#pragma once

#include <Windows.h>
#include "AppState.hpp"

namespace UI
{
    auto DrawMainWindow(HWND hWnd, APP_STATE& State) -> void;
}