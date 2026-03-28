# Lenovo DMI Decryptor

A Windows tool for analyzing, decrypting, and validating Lenovo DMI data in InsydeH2O firmware.

## Technical Details

### Data Layout

```c++
#pragma pack(push, 1)				// Ensure no padding between fields
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
	uint8_t			Signature[14];	// +0x00: "55 57 0E C2 69 11 56 4C A4 8A 98 24 AB 43 00 03" (decrypted)
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
```

### Encryption

DMI data is encrypted using a simple XOR scheme:

```c++
DecryptedByte = EncryptedByte ^ Key
```

### Integrity Check

The integrity check is a 16-bit additive checksum calculated over the encrypted body of the DMI data (excluding the header):

```c++
Checksum = Sum(Body) mod 2^16
```

## Tutorial

[![Tutorial](https://img.youtube.com/vi/zGtEksZGzxI/maxresdefault.jpg)](https://www.youtube.com/watch?v=zGtEksZGzxI)

## Precompiled Binary

For convenience, a precompiled binary is provided via the GitHub Releases page.