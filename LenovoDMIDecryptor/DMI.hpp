#pragma once

#include <Windows.h>
#include "AppState.hpp"

namespace Dmi
{
    auto ReloadBlocks(HWND hWnd, APP_STATE& State) -> bool;
	auto ToggleLdbgBlockEncryption(HWND hWnd, APP_STATE& State) -> bool;
	auto ToggleLenvBlock1Encryption(HWND hWnd, APP_STATE& State) -> bool;
	auto ToggleLenvBlock2Encryption(HWND hWnd, APP_STATE& State) -> bool;
	auto SetPrioritizedLenvBlock(HWND hWnd, APP_STATE& State, int BlockNumber) -> bool;
}