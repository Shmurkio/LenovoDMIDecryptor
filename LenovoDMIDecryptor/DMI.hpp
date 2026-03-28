#pragma once

#include <Windows.h>
#include "AppState.hpp"

namespace Dmi
{
    auto ReloadBlocks(HWND hWnd, APP_STATE& State) -> bool;
    auto GetBlockEncrypted(Lenovo::PDMI_DATA Block, bool& Encrypted, bool& Invalid) -> void;
}