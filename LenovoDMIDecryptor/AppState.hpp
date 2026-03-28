#pragma once

#include "File.hpp"
#include "Window.hpp"
#include "Lenovo.hpp"

struct HEX_VIEWER_STATE
{
    bool Open = false;
    bool PendingOpen = false;
    const uint8_t* Data = nullptr;
    uint32_t Size = 0;
};

struct APP_STATE
{
    File::FILE_INFO FileInfo{};
    Window::WINDOW_STATE Window{};

    Lenovo::PDMI_DATA DmiBlock1 = nullptr;
    bool DmiBlock1Found = false;

    Lenovo::PDMI_DATA DmiBlock2 = nullptr;
    bool DmiBlock2Found = false;

    uint32_t DmiBlockSize = 0;

    HEX_VIEWER_STATE HexViewer;
};