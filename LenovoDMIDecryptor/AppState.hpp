#pragma once

#include "File.hpp"
#include "Window.hpp"
#include "Lenovo.hpp"

struct APP_STATE
{
    File::FILE_INFO         FileInfo{};
    Window::WINDOW_STATE    Window{};

    bool                    LdbgBlockFound = false;
	Lenovo::PLDBG_BLOCK     LdbgBlock = nullptr;
    uint32_t                LdbgBlockSize = 0;
	uint32_t				LdbgBlockOffset = 0;
	bool                    LdbgBlockEncrypted = false;
	
    bool                    LenvBlock1Found = false;
    Lenovo::PLENV_BLOCK     LenvBlock1 = nullptr;
	uint32_t                LenvBlock1Size = 0;
	uint32_t				LenvBlock1Offset = 0;
	bool					LenvBlock1Encrypted = false;

    bool                    LenvBlock2Found = false;
    Lenovo::PLENV_BLOCK     LenvBlock2 = nullptr;
    uint32_t                LenvBlock2Size = 0;
	uint32_t				LenvBlock2Offset = 0;
	bool					LenvBlock2Encrypted = false;

	uint8_t					XorKey = 0;
};