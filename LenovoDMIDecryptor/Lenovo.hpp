#pragma once

#include "LenovoDMIDecryptor.hpp"

namespace Lenovo
{
#pragma pack(push, 1)					// Ensure no padding between fields
	typedef struct _DMI_HEADER
	{
		uint32_t		Signature;		// +0x00: "LENV"
		uint32_t		Unknown1;		// +0x04: Unknown
		uint32_t		Entries;		// +0x08: Number of DMI entries
		uint8_t			Unknown2;		// +0x0C: Unknown
		uint8_t			Key;			// +0x0D: XOR key
		uint16_t		Checksum;		// +0x0E: Additive 16-bit checksum of the body (excluding the header)
	} DMI_HEADER, *PDMI_HEADER;

	typedef struct _DMI_DATA_ENTRY
	{
		uint8_t			Signature[14];	// +0x00: "55 57 0E C2 69 11 56 4C A4 8A 98 24 AB 43" (decrypted)
		uint32_t		Type;			// +0x0E: Type of the entry (e.g. 0x0100 for motherboard name)
		uint64_t		DataSize;		// +0x12: Size of the data
		uint8_t			Data[1];		// +0x1A: Actual data
	} DMI_DATA_ENTRY, *PDMI_DATA_ENTRY;

	typedef struct _DMI_DATA
	{
		DMI_HEADER		Header;			// +0x00: DMI header
		uint8_t			Unknown[32];	// +0x10: Unknown
		DMI_DATA_ENTRY	Entries[1];		// +0x30: Array of DMI entries
	} DMI_DATA, *PDMI_DATA;
#pragma pack(pop)

	using CDMI_HEADER		= const DMI_HEADER;
	using PCDMI_HEADER		= const DMI_HEADER*;
	using CDMI_DATA_ENTRY	= const DMI_DATA_ENTRY;
	using PCDMI_DATA_ENTRY	= const DMI_DATA_ENTRY*;
	using CDMI_DATA			= const DMI_DATA;
	using PCDMI_DATA		= const PDMI_DATA*;

	constexpr const char* DMI_HEADER_PATTERN = "4C 45 4E 56 ? ? ? ? ? ? 00 00"; // DMI_HEADER::Entries shouldn't exceed uint16_t, therefore the upper two bytes should be zero
}