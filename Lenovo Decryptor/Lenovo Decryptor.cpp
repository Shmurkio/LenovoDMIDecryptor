#include "Header.h"

int main(int argc, char** argv) {
    SetConsoleTitleA("Lenovo DMI Decryption Tool | v1.3");

    if (argc < 2) {
        return 1;
    }

    std::string fileName = argv[1];

    std::string hexData = readHexData(fileName);

    if (hexData.empty()) {
        return 1;
    }

    int lenvOffset1 = findLenvOccurrences(hexData);
    int lenvOffset2 = lenvOffset1 + findLenvOccurrences(hexData.substr(lenvOffset1 + 2)) + 2;

    if (lenvOffset2 == 0) {
        return 1;
    }

    std::string key = hexData.substr((lenvOffset1 + 26), 2);

    std::string lenvRegion1 = hexData.substr((lenvOffset1 + 32), (lenvOffset2 - (lenvOffset1 + 32)));
    std::string lenvRegion2 = hexData.substr((lenvOffset2 + 32), lenvRegion1.length());

    std::string cryptedLenvRegion1 = xorEncryptDecrypt(lenvRegion1, key);
    std::string cryptedLenvRegion2 = xorEncryptDecrypt(lenvRegion2, key);

    std::string checksum16LenvRegion1;
    std::string checksum16LenvRegion2;

    if (cryptedLenvRegion1.substr(cryptedLenvRegion1.length() - 2) == key) {
        checksum16LenvRegion1 = getChecksum16(lenvRegionToBytesArray(cryptedLenvRegion1));
        checksum16LenvRegion2 = getChecksum16(lenvRegionToBytesArray(cryptedLenvRegion2));

        hexData = hexData.replace((lenvOffset1 + 28), 4, checksum16LenvRegion1);
        hexData = hexData.replace((lenvOffset2 + 28), 4, checksum16LenvRegion2);
    }

    hexData = hexData.replace((lenvOffset1 + 32), lenvRegion1.length(), cryptedLenvRegion1);
    hexData = hexData.replace((lenvOffset2 + 32), lenvRegion2.length(), cryptedLenvRegion2);

    writeHexDataToFile(fileName, hexData);

    if (cryptedLenvRegion1.substr(cryptedLenvRegion1.length() - 2) == key) {
        std::cout << "Success: DMI successfully encrypted with key " << key << ", checksum updated to " << checksum16LenvRegion1 << "/" << checksum16LenvRegion2 << "." << std::endl << std::endl;
    }
    else {
        std::cout << "Success: DMI successfully decrypted with key " << key << "." << std::endl << std::endl;
    }

    std::cout << "Press any key to close...";

    system("PAUSE >NUL 2>NUL");

    return 0;
}
