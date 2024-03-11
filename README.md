# Lenovo DMI Decryptor

## Syntax v1.3.1

![Lenovo DMI Decryptor v1.3.1 Syntaxt](https://i.imgur.com/lCqmmpL.png)

`LENV.exe "BIOS.bin" -info ["Info.txt"]` Prints info in console (or to "Info.txt" if added)

`LENV.exe "BIOS.bin" -decrypt ["BIOS_DECRYPTED.bin"]` Decrypts DMI region and writes results to input file (or to "BIOS_DECRYPTED.bin" if added)

`LENV.exe "BIOS.bin" -encrypt ["BIOS_ENCRYPTED.bin"]` Encrypts DMI region and writes results to input file (or to "BIOS_ENCRYPTED.bin" if added)

`LENV.exe "BIOS.bin" -extract "BIOS_DMI.bin"` Extracts DMI region to file "BIOS_DMI.bin"

`LENV.exe "BIOS.bin" -transfer "BIOS_TARGET.bin"` Transfers DMI region from "BIOS.bin" to "BIOS_TARGET.bin" (Note: DMI regions size has to match. Only use the same model/check if the encryption/decryption byte matches)

## Decrypted Lenovo DMI block structure reversal

![Analyzed DMI block](https://i.imgur.com/vt8awkd.png)

This is 1 of the 2 DMI blocks in a Lenovo BIOS dump which was decrypted. Analyzing and reverse engineering multiple BIOS dumps gives a general structure for all BIOSes.

`4C454E56` **LENV signature:** Initializes DMI block
`1 byte (8D)` **Unknown**
`3 bytes (00)` **Padding**
`1 byte (10)` **Number of data string:** 0x10 = 16 data strings in this DMI block
`4 bytes (00)` **Padding**
`1 byte (2A)` **Encryption/Decryption byte:** Byte used to XOR the data:
```c++
std::string xorEncryptDecrypt(std::string lenvRegion, std::string key) {
    std::stringstream result;

    for (size_t i = 0; i < lenvRegion.size(); i += 2) {
        int byte = std::stoi(lenvRegion.substr(i, 2), nullptr, 16);
        int keyByte = std::stoi(key.substr(i % key.size(), 2), nullptr, 16);
        int cryptedByte = byte ^ keyByte;

        result << std::hex << std::setw(2) << std::setfill('0') << cryptedByte;
    }

    return result.str();
}
```
`2 bytes (46EE)` **Checksum:** Checksum to verify the integrity of the data. Sum up all bytes to create a 16-bit checksum and swap the first and second byte, in this case 0xEE46 -> 46EE:
```c++
std::string getChecksum16(std::vector<uint8_t> bytesArray) {
    uint16_t checksum16 = 0;

    for (auto& b : bytesArray) {
        checksum16 += b;
    }

    checksum16 = (checksum16 >> 8) | (checksum16 << 8);
    std::stringstream checksum16Stream;
    checksum16Stream << std::hex << std::setw(4) << std::setfill('0') << checksum16;

    return checksum16Stream.str();
}
```
```20 bytes (321C5C468F4464236E88429349FDD887C40DE108)``` **Unknown**
`15 bytes (00)` **Padding**
`14 bytes (always 321C5C468F4464236E88429349FDD887C40DE108)` **Data fields separator:** Separates each "field" containing a data string
`2 bytes` **Data identifier:** Identify the data string. Byte types are for example:
- `0005` for UUID
- `0001` for motherboard name
- `0B00` for OA3 key ID
- 
`1 byte` **Length of the data string:** 0x0D = 13 = **n**
`7 bytes (00)` **Padding**
`n bytes` **Data string:** length **n**

Exception: Identifier `0100310000000000000001000000000000000100000000000000` for Windows key, followed by 1 byte for the key length, a 3 byte padding and then the key.

**Make sure to fix the byte for the data string length too if you change the length of the data string.**
