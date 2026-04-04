#pragma once

#include "LenovoDMIDecryptor.hpp"
#include "Cast.hpp"

namespace Lenovo
{
	constexpr auto Signature32(uint8_t A, uint8_t B, uint8_t C, uint8_t D) -> uint32_t
	{
		return (Cast::To<uint32_t>(D) << 24) | (Cast::To<uint32_t>(C) << 16) | (Cast::To<uint32_t>(B) << 8) | Cast::To<uint32_t>(A);
	};

#pragma pack(push, 1)
	typedef struct _LENV_HEADER
	{
		uint32_t				Signature;		// +0x00: "LENV"
		uint32_t				Generation;		// +0x04: Higher value = newer block, 0 = invalid
		uint32_t				Entries;		// +0x08: Total number of entries
		uint8_t					AccessFlag;		// +0x0C: Bit 0 appears to mark the block as write-protected
		uint8_t					XorKey;			// +0x0D: Each byte of the block body (i.e. all bytes after the header) is XORed with this key when the block is encrypted
		uint16_t				Checksum;		// +0x0E: Additive 16-bit checksum of the encrypted block body
	} LENV_HEADER, *PLENV_HEADER;

	typedef struct _NAMESPACE_ID
	{
		uint8_t					Bytes[0x0E];
	} NAMESPACE_ID, *PNAMESPACE_ID;

	using ENTRY_TYPE = uint16_t;

	typedef struct _ENTRY_KEY					
	{
		NAMESPACE_ID			NamespaceId;	// +0x00: Used to group entries in different categories
		ENTRY_TYPE				Type;			// +0x0E: Specifies the type of entry within the namespace
	} ENTRY_KEY, *PENTRY_KEY;

	typedef struct _LENV_ENTRY
	{
		ENTRY_KEY				Key;			// +0x00: Composite key used by the firmware to identify entries within a certain namespace
		uint32_t				DataSize;		// +0x10: Size of the Data field in bytes
		uint8_t					Flags;			// +0x14: Bit 0 appears to mark the entry as write-protected
		uint8_t					Unknown1;		// +0x15: Unknown
		uint16_t				Unknown2;		// +0x16: Unknown
		uint8_t					Data[1];		// +0x18: Actual data
	} LENV_ENTRY, *PLENV_ENTRY;

	typedef struct _LENV_BLOCK
	{
		LENV_HEADER				Header;			// +0x00: LENV block header
		LENV_ENTRY				Entries[1];		// +0x10: Array of LENV entries
	} LENV_BLOCK, *PLENV_BLOCK;

	typedef struct _LDBG_HEADER
	{
		uint32_t				Signature;		// +0x00: "LDBG"
		uint32_t				WriteOffset;	// +0x04: Used by the firmware to determine where to append new entries within the block
		uint8_t					Unknown1[0x18];	// +0x08: Unknown. Doesn't appear to serve any purpose in the reverse-engineered firmware
	} LDBG_HEADER, *PLDBG_HEADER;

	typedef struct _TIMESTAMP
	{
		uint16_t				Year;			// +0x00: Year is stored as (2000 + RTC_year_byte), so only the low byte contains the RTC BCD year value
		uint8_t					Month;			// +0x02: BCD encoded month value read from RTC
		uint8_t					Day;			// +0x03: BCD encoded day value read from RTC
		uint8_t					Hour;			// +0x04: BCD encoded hour value read from RTC
		uint8_t					Minute;			// +0x05: BCD encoded minute value read from RTC
		uint8_t					Second;			// +0x06: BCD encoded second value read from RTC
	} TIMESTAMP, *PTIMESTAMP;

	enum class LDBG_OPERATION : uint8_t
	{
		SetData		= 0x02,
		Protect		= 0x06,
		Unprotect	= 0x07
	};

	typedef struct _LDBG_ENTRY
	{
		TIMESTAMP				Timestamp;		// +0x00: Timestamp of the entry
		LDBG_OPERATION			Operation;		// +0x07: Operation performed on the entry. The firmware appears to only create entries with operations SetData, Protect, and Unprotect
		ENTRY_KEY				Key;			// +0x08: Composite key of the entry
		uint32_t				Size;			// +0x10: Size of the entry data
		uint8_t					Unknown1[0x4];	// +0x14: Unknown. Doesn't appear to serve any purpose in the reverse-engineered firmware
	} LDBG_ENTRY, *PLDBG_ENTRY;
	
	typedef struct _LDBG_BLOCK
	{
		LDBG_HEADER				Header;			// +0x00: LDBG block header
		LDBG_ENTRY				Entries[1];		// +0x20: Array of LDBG entries
	} LDBG_BLOCK, *PLDBG_BLOCK;
#pragma pack(pop)

	using CLENV_HEADER		= const LENV_HEADER;
	using PCLENV_HEADER		= const LENV_HEADER*;
	using CNAMESPACE_ID		= const NAMESPACE_ID;
	using PCNAMESPACE_ID	= const NAMESPACE_ID*;
	using PENTRY_TYPE		= ENTRY_TYPE*;
	using CENTRY_TYPE		= const ENTRY_TYPE;
	using PCENTRY_TYPE		= const ENTRY_TYPE*;
	using CENTRY_KEY		= const ENTRY_KEY;
	using PCENTRY_KEY		= const ENTRY_KEY*;
	using CLENV_ENTRY		= const LENV_ENTRY;
	using PCLENV_ENTRY		= const LENV_ENTRY*;
	using CLENV_BLOCK		= const LENV_BLOCK;
	using PCLENV_BLOCK		= const LENV_BLOCK*;
	using CLDBG_HEADER		= const LDBG_HEADER;
	using PCLDBG_HEADER		= const LDBG_HEADER*;
	using CTIMESTAMP		= const TIMESTAMP;
	using PCTIMESTAMP		= const TIMESTAMP*;
	using CLDBG_ENTRY		= const LDBG_ENTRY;
	using PCLDBG_ENTRY		= const LDBG_ENTRY*;

	constexpr const uint32_t LENV_SIGNATURE = Signature32('L', 'E', 'N', 'V');
	constexpr const uint32_t LDBG_SIGNATURE = Signature32('L', 'D', 'B', 'G');

	constexpr const uint32_t DEFAULT_LENV_SIZE = 0x1000;
	constexpr const uint32_t DEFAULT_LDBG_SIZE = 0x2000;

	constexpr const char* LDBG_PATTERN = "4C 44 42 47 ? ? ? ? 00 00 00 00 00 00 00 00";
}