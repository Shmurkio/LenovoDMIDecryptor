#include "DMI.hpp"

#include <tchar.h>

#include "Util.hpp"
#include "Cast.hpp"
#include "Lenovo.hpp"

auto Dmi::GetBlockEncrypted(Lenovo::PDMI_DATA Block, bool& Encrypted, bool& Invalid) -> void
{
    Encrypted = false;
    Invalid = false;

    if (!Block)
    {
        return;
    }

    auto Ptr = reinterpret_cast<uint8_t*>(Block);
    const auto Value = Ptr[Lenovo::DEFAULT_DMI_BLOCK_SIZE - 1];

    if (Value == 0)
    {
        Encrypted = false;
    }
    else if (Value == Block->Header.Key)
    {
        Encrypted = true;
    }
    else
    {
        Invalid = true;
    }
}

auto Dmi::ReloadBlocks(HWND hWnd, APP_STATE& State) -> bool
{
    State.DmiBlock1 = nullptr;
    State.DmiBlock2 = nullptr;
    State.DmiBlock1Found = false;
    State.DmiBlock2Found = false;
    State.DmiBlockSize = 0;

    auto Status = Util::FindPattern(
        State.FileInfo.Buffer,
        State.FileInfo.Size,
        Lenovo::DMI_HEADER_PATTERN,
        State.DmiBlock1
    );

    if (Status != STATUS_SUCCESS || !State.DmiBlock1)
    {
        MessageBox(
            hWnd,
            _T("Failed to find the first DMI header in the selected file."),
            _T("Error"),
            MB_ICONERROR
        );
        return false;
    }

    State.DmiBlock1Found = true;

    const auto Header1Offset =
        Cast::To<uint64_t>(Cast::To<uint8_t*>(State.DmiBlock1) - State.FileInfo.Buffer);

    auto SearchStart =
        Cast::To<uint8_t*>(State.DmiBlock1) + sizeof(Lenovo::DMI_HEADER);

    const auto RemainingSize =
        State.FileInfo.Size - (Header1Offset + sizeof(Lenovo::DMI_HEADER));

    Status = Util::FindPattern(
        SearchStart,
        RemainingSize,
        Lenovo::DMI_HEADER_PATTERN,
        State.DmiBlock2
    );

    if (Status != STATUS_SUCCESS || !State.DmiBlock2)
    {
        MessageBox(
            hWnd,
            _T("Failed to find the second DMI header in the selected file."),
            _T("Error"),
            MB_ICONERROR
        );

        State.DmiBlock1 = nullptr;
        State.DmiBlock2 = nullptr;
        State.DmiBlock1Found = false;
        State.DmiBlock2Found = false;
        State.DmiBlockSize = 0;

        return false;
    }

    State.DmiBlock2Found = true;

    State.DmiBlockSize = static_cast<uint32_t>(
        Cast::To<uint64_t>(State.DmiBlock2) -
        Cast::To<uint64_t>(State.DmiBlock1)
    );

    return true;
}