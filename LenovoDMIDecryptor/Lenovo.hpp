#pragma once

#include "LenovoDMIDecryptor.hpp"

namespace Lenovo
{
#pragma pack(push, 1)							// Ensure no padding between fields
	typedef struct _DMI_HEADER
	{
		uint32_t		Signature;				// +0x00: "LENV"
		uint32_t		Generation;				// +0x04: "Generation" - LenovoVariableDxe prefers the newer block (highest generation). If a block's generation is 0, the block is considered invalid. Error if both blocks have 0 generation.
		uint32_t		Entries;				// +0x08: Number of DMI entries
		uint8_t			AccessFlag;				// +0x0C: 1 == read-only
		uint8_t			Key;					// +0x0D: XOR key
		uint16_t		Checksum;				// +0x0E: Additive 16-bit checksum of the body (excluding the header)
	} DMI_HEADER, *PDMI_HEADER;

	typedef struct _DMI_ENTRY_KEY
	{
		uint8_t			Signature[0x0E];		// +0x00: Lenovo DMI entry signature / namespace identifier ("55 57 0E C2 69 11 56 4C A4 8A 98 24 AB 43" consumed by L05SmbiosOverride to retrieve SMBIOS data, "46 8F 44 64 23 6E 88 42 93 49 FD D8 87 C4" consumed by BdsDxe and OneKeyRecovery)
		uint16_t		FieldId;				// +0x0E: Per-entry identifier; together with Signature forms the lookup key for LENOVO_VARIABLE_PROTOCOL
	} DMI_ENTRY_KEY, *PDMI_ENTRY_KEY;

	typedef struct _DMI_DATA_ENTRY
	{
		DMI_ENTRY_KEY	Key;					// +0x00: Lookup key used by LENOVO_VARIABLE_PROTOCOL
		uint32_t		DataSize;				// +0x10: Size of the data
		uint8_t			Flags;					// +0x14: bit0 appears to mark the entry as write-protected
		uint8_t			Unknown1;				// +0x15: Unknown
		uint16_t		Unknown2;				// +0x16: Unknown
		uint8_t			Data[1];				// +0x18: Actual data
	} DMI_DATA_ENTRY, *PDMI_DATA_ENTRY;

	typedef struct _DMI_DATA
	{
		DMI_HEADER		Header;					// +0x00: DMI header
		DMI_DATA_ENTRY	Entry[1];				// +0x10: Array of DMI entries
	} DMI_DATA, *PDMI_DATA;

	typedef struct _DMI_DATA_ENTRY_WINDOWS_KEY	// Unknown1 or Unknown2 could represent a "present" flag or a number of keys, not too sure about this
	{
		uint64_t		Unknown1;				// 0x00: Unknown
		uint64_t		Unknown2;				// 0x08: Unknown
		uint32_t		KeyLength;				// 0x10: Length of the Windows key
		char			Key[1];					// 0x14: Windows key
	} DMI_DATA_ENTRY_WINDOWS_KEY, *PDMI_DATA_ENTRY_WINDOWS_KEY;
#pragma pack(pop)

	using CDMI_HEADER					= const DMI_HEADER;
	using PCDMI_HEADER					= const DMI_HEADER*;
	using CDMI_DATA_ENTRY				= const DMI_DATA_ENTRY;
	using PCDMI_DATA_ENTRY				= const DMI_DATA_ENTRY*;
	using CDMI_DATA						= const DMI_DATA;
	using PCDMI_DATA					= const DMI_DATA*;
	using CDMI_DATA_ENTRY_WINDOWS_KEY	= const DMI_DATA_ENTRY_WINDOWS_KEY;
	using PCDMI_DATA_ENTRY_WINDOWS_KEY	= const DMI_DATA_ENTRY_WINDOWS_KEY*;

	constexpr const uint32_t DEFAULT_DMI_BLOCK_SIZE = 0x1000; // LenovoVariableDxe allocates 0x1000 bytes / 1 page for the DMI block, so it's unlikely that the actual size exceeds this

	constexpr const char* DMI_HEADER_PATTERN = "4C 45 4E 56 ? ? ? ? ? ? 00 00"; // DMI_HEADER::Entries shouldn't exceed uint16_t, therefore the upper two bytes should be zero

	// FieldIds used within Lenovo's proprietary DMI storage.
	// A DMI entry is identified by a composite key: Signature[14] + FieldId.
	// The Signature acts as a namespace identifier, FieldId selects a specific field.
	// LENOVO_VARIABLE_PROTOCOL uses this composite key to retrieve entry data.
	// Some consumers appear to rely on fixed positions, therefore changing the order or length of entries
	// may break the mapping.
	constexpr const uint16_t WINDOWS_KEY				= 0x0001;
	constexpr const uint16_t MOTHERBOARD_NAME			= 0x0100;
	constexpr const uint16_t OA3_KEY_ID					= 0x000B;
	constexpr const uint16_t BASEBOARD_PLATFORM_ID		= 0x0F00;
	constexpr const uint16_t OS_PRELOAD_SUFFIX			= 0x1000;
	constexpr const uint16_t BASEBOARD_SERIAL_NUMBER	= 0x0400;
	constexpr const uint16_t MACHINE_TYPE_MODEL			= 0x0200;
}