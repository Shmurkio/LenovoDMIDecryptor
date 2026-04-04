# Lenovo DMI Decryptor

A Windows tool for analyzing, decrypting, and reverse engineering Lenovo's proprietary firmware storage (`LENV`) used in InsydeH2O BIOS images.

---

## Overview

Lenovo firmware implements a custom variable storage system identified by the signature:

```
'LENV'
```

This storage is accessed via the proprietary:

```
LENOVO_VARIABLE_PROTOCOL
```

and is used by multiple DXE drivers to store and retrieve system configuration, SMBIOS data, and other platform-specific information.

In addition to this storage, a second structure exists:

```
'LDBG'
```

which acts as a change log, tracking modifications to LENV entries over time.

---

## Storage Layout

The firmware stores three regions at fixed physical addresses:

```
+------------------------+
| LDBG Block (0x2000)    |
+------------------------+
| LENV Block 1 (0x1000)  |
+------------------------+
| LENV Block 2 (0x1000)  |
+------------------------+
```

This layout is hard-coded inside `LenovoVariableDxe`:

```c++
EFI_STATUS __fastcall GetLenvStorageLayout(
        EFI_PHYSICAL_ADDRESS *DmiBlock1Physical,
        UINT32 *DmiBlock1Size,
        EFI_PHYSICAL_ADDRESS *DmiBlock2Physical,
        UINT32 *DmiBlock2Size,
        EFI_PHYSICAL_ADDRESS *LdbgBlockPhysical,
        UINT32 *LdbgBlockSize)
{
  *DmiBlock1Physical = 0xFF622000;
  *DmiBlock1Size = 0x1000;
  *DmiBlock2Physical = 0xFF623000;
  *DmiBlock2Size = 0x1000;
  *LdbgBlockPhysical = 0xFF620000;
  *LdbgBlockSize = 0x2000;
  return 0i64;
}
```

Two redundant `LENV` blocks are maintained. The firmware selects the active one based on:

- **Generation**: higher is newer
- **Checksum validity**: must be valid

---

## LENV Block

### Header

```c++
typedef struct _LENV_HEADER
{
	uint32_t				Signature;		// +0x00: "LENV"
	uint32_t				Generation;		// +0x04: Higher value = newer block, 0 = invalid
	uint32_t				Entries;		// +0x08: Total number of entries
	uint8_t					AccessFlag;		// +0x0C: Bit 0 appears to mark the block as write-protected
	uint8_t					XorKey;			// +0x0D: Each byte of the block body (i.e. all bytes after the header) is XORed with this key when the block is encrypted
	uint16_t				Checksum;		// +0x0E: Additive 16-bit checksum of the encrypted block body
} LENV_HEADER, *PLENV_HEADER;
```

### Entries

```c++
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
```

---

## Composite Lookup Key

Entries are not indexed by SMBIOS type.

Instead, each entry is identified by:

```
NamespaceId (14 bytes) + Type (2 bytes)
```

This forms the lookup key used by `LENOVO_VARIABLE_PROTOCOL`.

---

## Encryption

The LENV block uses a simple XOR scheme:

```
DecryptedByte = EncryptedByte ^ XorKey
```

- Only the **body** is encrypted (everything after the header)
- The key is stored in:

  ```
  Header->XorKey
  ```

---

## Integrity Check

The checksum is a 16-bit additive checksum:

```
Checksum = sum(encrypted_body) mod 2^16
```

- Computed over the **encrypted body**
- Header is excluded

---

## LDBG Block (Debug Log)

The LDBG block records changes made to LENV storage.

### Header

```c++
typedef struct _LDBG_HEADER
{
	uint32_t				Signature;		// +0x00: "LDBG"
	uint32_t				WriteOffset;	// +0x04: Used by the firmware to determine where to append new entries within the block
	uint8_t					Unknown1[0x18];	// +0x08: Unknown. Doesn't appear to serve any purpose in the reverse-engineered firmware
} LDBG_HEADER, *PLDBG_HEADER;
```

---

### Entries

```c++
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
```

---

### Operations

| Value | Meaning   |
| ----- | --------- |
| 0x02  | SetData   |
| 0x06  | Protect   |
| 0x07  | Unprotect |

---

### Timestamp

- Read from CMOS RTC
- Stored in **BCD format**
- Year stored as:

  ```
  2000 + RTC_year_byte
  ```

---

## Firmware Interface

Access is performed via:

```
LENOVO_VARIABLE_PROTOCOL
```

- GUID: `BFD02359-8DFE-459A-8B69-A73A6BAFADC0`
- Installed by: `LenovoVariableDxe`
- Backed by SMM: `LenovoVariableSmm`

---

## SMBIOS Mapping

Mapping appears to be handled by `L05SmbiosOverride`.

It is:

- Complex
- Not purely type-based
- Partially hard-coded
- Partially offset-based

Modifying entry layout or size likely breaks SMBIOS population.

---

## Showcase

[![Showcase](https://img.youtube.com/vi/0lcOLBSktoQ/maxresdefault.jpg)](https://www.youtube.com/watch?v=0lcOLBSktoQ)

---

## Disclaimer

This project is based entirely on reverse-enginerred Lenovo InsydeH2O firmware.

There is no guarantee that it will work correctly for all Lenovo InsydeH2O firmware images.

I am not responsible for any damage, data loss, or other issues caused by the use of this software.

---

## Precompiled Binary

A precompiled binary is available via the GitHub Releases page.