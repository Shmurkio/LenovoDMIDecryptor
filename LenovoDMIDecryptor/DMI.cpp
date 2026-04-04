#include "DMI.hpp"

#include <tchar.h>

#include "Util.hpp"
#include "Cast.hpp"
#include "Lenovo.hpp"

constexpr bool IsInRange(uint64_t Value, uint64_t Base, uint64_t Size)
{
    if (Value < Base)
    {
        return false;
    }

    return (Value - Base) < Size;
}

auto Dmi::ReloadBlocks(HWND hWnd, APP_STATE& State) -> bool
{
	// Reset block states.
	State.LdbgBlockFound        = false;
	State.LdbgBlock             = nullptr;
	State.LdbgBlockSize         = 0;
	State.LdbgBlockOffset       = 0;
	State.LdbgBlockEncrypted    = false;

	State.LenvBlock1Found       = false;
	State.LenvBlock1            = nullptr;
	State.LenvBlock1Size        = 0;
	State.LenvBlock1Offset      = 0;
	State.LenvBlock1Encrypted   = false;

	State.LenvBlock2Found       = false;
    State.LenvBlock2            = nullptr;
    State.LenvBlock2Size        = 0;
	State.LenvBlock2Offset      = 0;
    State.LenvBlock2Encrypted   = false;

	State.XorKey                = 0;

	// Find LDBG block.
	Lenovo::PLDBG_BLOCK LdbgBlock = nullptr;

    auto Status = Util::FindPattern(
        State.FileInfo.Buffer,
        State.FileInfo.Size,
        Lenovo::LDBG_PATTERN,
        LdbgBlock
    );

    if (Status != STATUS_SUCCESS)
    {
        MessageBox(
            hWnd,
            _T("Failed to find the LDBG block in the selected file."),
            _T("Error"),
            MB_ICONERROR
        );

		return false;
    }

    // Use hardcoded layout to find the two LENV blocks.
    auto LenvBlock1 = Cast::To<Lenovo::PLENV_BLOCK>(
        Cast::To<uint8_t*>(LdbgBlock) + Lenovo::DEFAULT_LDBG_SIZE
	);

    if (!IsInRange(Cast::To<uint64_t>(LenvBlock1), Cast::To<uint64_t>(State.FileInfo.Buffer), State.FileInfo.Size) || LenvBlock1->Header.Signature != Lenovo::LENV_SIGNATURE)
    {
        MessageBox(
            hWnd,
            _T("Failed to find the first LENV block in the selected file."),
            _T("Error"),
            MB_ICONERROR
        );

        return false;
    }

    auto LenvBlock2 = Cast::To<Lenovo::PLENV_BLOCK>(
        Cast::To<uint8_t*>(LenvBlock1) + Lenovo::DEFAULT_LENV_SIZE
	);

    if (!IsInRange(Cast::To<uint64_t>(LenvBlock2), Cast::To<uint64_t>(State.FileInfo.Buffer), State.FileInfo.Size) || LenvBlock2->Header.Signature != Lenovo::LENV_SIGNATURE)
    {
        MessageBox(
            hWnd,
            _T("Failed to find the second LENV block in the selected file."),
            _T("Error"),
            MB_ICONERROR
        );

        return false;
	}

    // Check if the blocks are encrypted.
	auto LdbgBlockEncrypted = (
        Cast::To<uint8_t*>(LdbgBlock)[Lenovo::DEFAULT_LDBG_SIZE - 1] != 0
	);

    auto LenvBlock1Encrypted = (
        Cast::To<uint8_t*>(LenvBlock1)[Lenovo::DEFAULT_LENV_SIZE - 1] != 0
    );

    auto LenvBlock2Encrypted = (
        Cast::To<uint8_t*>(LenvBlock2)[Lenovo::DEFAULT_LENV_SIZE - 1] != 0
	);

	// Update state.
	State.LdbgBlockFound        = true;
	State.LdbgBlock             = LdbgBlock;
	State.LdbgBlockSize         = Lenovo::DEFAULT_LDBG_SIZE;
	State.LdbgBlockOffset       = Cast::To<uint32_t>(Cast::To<uint8_t*>(LdbgBlock) - State.FileInfo.Buffer);
	State.LdbgBlockEncrypted    = LdbgBlockEncrypted;

	State.LenvBlock1Found       = true;
	State.LenvBlock1            = LenvBlock1;
	State.LenvBlock1Size        = Lenovo::DEFAULT_LENV_SIZE;
	State.LenvBlock1Offset      = Cast::To<uint32_t>(Cast::To<uint8_t*>(LenvBlock1) - State.FileInfo.Buffer);
	State.LenvBlock1Encrypted   = LenvBlock1Encrypted;

	State.LenvBlock2Found       = true;
	State.LenvBlock2            = LenvBlock2;
	State.LenvBlock2Size        = Lenovo::DEFAULT_LENV_SIZE;
	State.LenvBlock2Offset      = Cast::To<uint32_t>(Cast::To<uint8_t*>(LenvBlock2) - State.FileInfo.Buffer);
	State.LenvBlock2Encrypted   = LenvBlock2Encrypted;

	State.XorKey                = LenvBlock1->Header.XorKey;

	return true;
}

auto Dmi::ToggleLdbgBlockEncryption(HWND hWnd, APP_STATE& State) -> bool
{
    auto Body = Cast::To<uint8_t*>(State.LdbgBlock) + sizeof(Lenovo::LDBG_HEADER);
    auto BodySize = static_cast<size_t>(State.LdbgBlockSize - sizeof(Lenovo::LDBG_HEADER));

    if (!Util::XorBuffer(Body, BodySize, State.XorKey))
    {
        MessageBox(hWnd, _T("Failed to toggle LDBG block encryption."), _T("Error"), MB_ICONERROR);
        return false;
    }

    State.FileInfo.Dirty = true;

    return ReloadBlocks(hWnd, State);
}

auto Dmi::ToggleLenvBlock1Encryption(HWND hWnd, APP_STATE& State) -> bool
{
    auto Body = Cast::To<uint8_t*>(State.LenvBlock1) + sizeof(Lenovo::LENV_HEADER);
    auto BodySize = static_cast<size_t>(State.LenvBlock1Size - sizeof(Lenovo::LENV_HEADER));

    if (!Util::XorBuffer(Body, BodySize, State.XorKey))
    {
        MessageBox(hWnd, _T("Failed to toggle LENV Block 1 encryption."), _T("Error"), MB_ICONERROR);
        return false;
    }

    State.LenvBlock1->Header.Checksum = Util::GetChecksum16(Body, BodySize);
    State.FileInfo.Dirty = true;

	return ReloadBlocks(hWnd, State);
}

auto Dmi::ToggleLenvBlock2Encryption(HWND hWnd, APP_STATE& State) -> bool
{
    auto Body = Cast::To<uint8_t*>(State.LenvBlock2) + sizeof(Lenovo::LENV_HEADER);
    auto BodySize = static_cast<size_t>(State.LenvBlock2Size - sizeof(Lenovo::LENV_HEADER));

    if (!Util::XorBuffer(Body, BodySize, State.XorKey))
    {
        MessageBox(hWnd, _T("Failed to toggle LENV Block 2 encryption."), _T("Error"), MB_ICONERROR);
        return false;
    }

    State.LenvBlock2->Header.Checksum = Util::GetChecksum16(Body, BodySize);
    State.FileInfo.Dirty = true;

    return ReloadBlocks(hWnd, State);
}

auto Dmi::SetPrioritizedLenvBlock(HWND hWnd, APP_STATE& State, int BlockNumber) -> bool
{
    auto& Generation1 = State.LenvBlock1->Header.Generation;
    auto& Generation2 = State.LenvBlock2->Header.Generation;

    const auto Block1Prioritized = (Generation1 > Generation2);
    const auto Block2Prioritized = (Generation2 > Generation1);

    if ((BlockNumber == 1 && Block1Prioritized) || (BlockNumber == 2 && Block2Prioritized))
    {
        return true;
    }

    std::swap(Generation1, Generation2);

    State.FileInfo.Dirty = true;

    return Dmi::ReloadBlocks(hWnd, State);
}