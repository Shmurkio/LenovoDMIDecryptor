#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);