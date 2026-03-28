#pragma once

#include <cstdint>

#include "Cast.hpp"

#define PLATFORM_WINDOWS_UM

namespace Util
{
    auto IsHexCharacter(const char Character) -> bool;
    auto HexToNibble(const char Character) -> uint8_t;
    auto GetChecksum16(uint8_t* Buffer, uint64_t Size) -> uint16_t;
    auto XorBuffer(uint8_t* Buffer, size_t Size, uint8_t Key) -> bool;
}

#ifdef PLATFORM_WINDOWS_UM
#include <Windows.h>
#include <ntstatus.h>

namespace Util
{
    template<typename T, typename U>
    auto FindPattern(T BaseAddress, size_t Size, const char* Pattern, U& Out) -> NTSTATUS
    {
        Out = U{};

        if (!BaseAddress || !Size || !Pattern || !*Pattern)
        {
            return STATUS_INVALID_PARAMETER;
        }

        auto Start = Cast::To<uint8_t*>(BaseAddress);
        auto End = Start + Size;

        for (auto Current = Start; Current < End; ++Current)
        {
            const auto* Data = Current;
            auto PatternCursor = Pattern;

            while (*PatternCursor)
            {
                while (*PatternCursor == ' ')
                {
                    ++PatternCursor;
                }

                if (!*PatternCursor)
                {
                    Out = Cast::To<U>(Current);
                    return STATUS_SUCCESS;
                }

                if (Data >= End)
                {
                    break;
                }

                if (*PatternCursor == '?')
                {
                    ++Data;

                    if (PatternCursor[1] == '?')
                    {
                        PatternCursor += 2;
                    }
                    else
                    {
                        PatternCursor += 1;
                    }

                    continue;
                }

                // Need two hex characters for a byte like "48"
                if (!PatternCursor[0] || !PatternCursor[1])
                {
                    return STATUS_INVALID_PARAMETER;
                }

                if (!IsHexCharacter(PatternCursor[0]) || !IsHexCharacter(PatternCursor[1]))
                {
                    return STATUS_INVALID_PARAMETER;
                }

                const auto Byte =
                    Cast::To<uint8_t>(HexToNibble(PatternCursor[0]) << 4) |
                    HexToNibble(PatternCursor[1]);

                if (*Data != Byte)
                {
                    break;
                }

                ++Data;
                PatternCursor += 2;
            }

            while (*PatternCursor == ' ')
            {
                ++PatternCursor;
            }

            if (!*PatternCursor)
            {
                Out = Cast::To<U>(Current);
                return STATUS_SUCCESS;
            }
        }

        return STATUS_NOT_FOUND;
    }
}
#endif