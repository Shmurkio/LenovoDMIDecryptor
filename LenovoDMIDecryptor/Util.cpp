#include "Util.hpp"

auto Util::GetChecksum16(uint8_t* Buffer, uint64_t Size) -> uint16_t
{
    if (!Buffer || Size == 0)
    {
        return 0;
    }

    uint16_t Checksum = 0;

    for (uint64_t i = 0; i < Size; ++i)
    {
        Checksum = static_cast<uint16_t>(Checksum + Buffer[i]);
    }

    return Checksum;
}

auto Util::XorBuffer(uint8_t* Buffer, size_t Size, uint8_t Key) -> bool
{
    if (!Buffer || !Size)
    {
        return false;
    }

    for (size_t i = 0; i < Size; ++i)
    {
        Buffer[i] ^= Key;
    }

    return true;
}

auto Util::HexToNibble(const char Character) -> uint8_t
{
    if (Character >= '0' && Character <= '9')
    {
        return Cast::To<uint8_t>(Character - '0');
    }

    if (Character >= 'A' && Character <= 'F')
    {
        return Cast::To<uint8_t>(Character - 'A' + 10);
    }

    return Cast::To<uint8_t>(Character - 'a' + 10);
}

auto Util::IsHexCharacter(const char Character) -> bool
{
    return (Character >= '0' && Character <= '9') ||
        (Character >= 'A' && Character <= 'F') ||
        (Character >= 'a' && Character <= 'f');
}