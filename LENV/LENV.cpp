#include "Header.h"

int main(int argc, char* argv[]) {

    std::string programName = argv[0];

    size_t last_slash = programName.find_last_of("\\/");

    if (last_slash != std::string::npos) {
        programName = programName.substr(last_slash + 1);
    }

    if (argc < 3 || argc > 4) {
        std::cerr << "Version:\t1.3.1" << std::endl
            << "Usage:\t\t" << programName << " \"Input file\" <-encrypt, -decrypt, -extract, -info> [\"Output file\"]" << std::endl
            << "\t\t" << programName << " \"DMI source file\" -transfer \"Target file\"" << std::endl;

        return 1;
    }

    std::string fileName = argv[1];

    std::string outFileName;

    if (argc == 4) {
        outFileName = argv[3];
    }
    else {
        outFileName = fileName;
    }

    std::string hexData = readHexData(fileName);

    if (hexData.empty()) {
        return 1;
    }

    int lenvOffset1 = findLenvOccurrences(hexData);

    if (lenvOffset1 == -1) {
        std::cerr << "DMI block 1 not found in file \"" << fileName << "\"." << std::endl;

        return 1;
    }

    int lenvOffset2 = lenvOffset1 + findLenvOccurrences(hexData.substr(lenvOffset1 + 2)) + 2;

    if (lenvOffset2 == -1) {
        std::cerr << "DMI block 2 not found in file \"" << fileName << "\"." << std::endl;
        return 1;
    }

    std::string key = hexData.substr((lenvOffset1 + 26), 2);

    std::string lenvRegion1 = hexData.substr((lenvOffset1 + 32), (lenvOffset2 - (lenvOffset1 + 32)));
    std::string lenvRegion2 = hexData.substr((lenvOffset2 + 32), lenvRegion1.length());

    std::string cryptedLenvRegion1 = xorEncryptDecrypt(lenvRegion1, key);
    std::string cryptedLenvRegion2 = xorEncryptDecrypt(lenvRegion2, key);

    std::string checksum16LenvRegion1 = getChecksum16(lenvRegionToBytesArray(cryptedLenvRegion1));
    std::string checksum16LenvRegion2 = getChecksum16(lenvRegionToBytesArray(cryptedLenvRegion2));

    int amountDataLenvRegion1 = std::stoi(hexData.substr((lenvOffset1 + 16), 2), nullptr, 16);
    int amountDataLenvRegion2 = std::stoi(hexData.substr((lenvOffset2 + 16), 2), nullptr, 16);

    std::string dmiBlock = hexData.substr(lenvOffset1, ((lenvOffset2 - lenvOffset1) * 2));

    uintmax_t fileSize = getFileSize(fileName);

    if (strcmp(argv[2], "-encrypt") == 0) {
        if (cryptedLenvRegion1.substr(cryptedLenvRegion1.length() - 2) != key) {
            std::cerr << "DMI blocks in file \"" << fileName << "\" are not decrypted." << std::endl;

            return 1;
        }



        hexData = hexData.replace((lenvOffset1 + 28), 4, checksum16LenvRegion1);
        hexData = hexData.replace((lenvOffset2 + 28), 4, checksum16LenvRegion2);

        hexData = hexData.replace((lenvOffset1 + 32), lenvRegion1.length(), cryptedLenvRegion1);
        hexData = hexData.replace((lenvOffset2 + 32), lenvRegion2.length(), cryptedLenvRegion2);

        writeHexDataToFile(outFileName, hexData);

        std::cout << "Success: DMI successfully encrypted with key " << key << ", checksum updated to " << checksum16LenvRegion1 << "/" << checksum16LenvRegion2 << "." << std::endl;
    }
    else if (strcmp(argv[2], "-decrypt") == 0) {
        if (cryptedLenvRegion1.substr(cryptedLenvRegion1.length() - 2) == key) {
            std::cerr << "DMI blocks in file \"" << fileName << "\" are not encrypted." << std::endl;

            return 1;
        }

        hexData = hexData.replace((lenvOffset1 + 32), lenvRegion1.length(), cryptedLenvRegion1);
        hexData = hexData.replace((lenvOffset2 + 32), lenvRegion2.length(), cryptedLenvRegion2);

        writeHexDataToFile(outFileName, hexData);

        std::cout << "Success: DMI successfully decrypted with key " << key << "." << std::endl;
    }
    else if (strcmp(argv[2], "-extract") == 0) {
        if (outFileName == fileName) {
            std::cerr << "Specify output file for extraction." << std::endl;

            return 1;
        }

        writeHexDataToFile(outFileName, hexData.substr(lenvOffset1, ((lenvOffset2 - lenvOffset1) * 2)));

        std::cout << "Success: DMI blocks exported to \"" << outFileName << "\"." << std::endl;
    }
    else if (strcmp(argv[2], "-transfer") == 0) {
        if (outFileName == fileName) {
            std::cerr << "Specify output file for transfering." << std::endl;

            return 1;
        }

        std::string hexDataOutFile = readHexData(outFileName);

        if (findLenvOccurrences(hexDataOutFile) == -1) {
            std::cerr << "DMI block 1 not found in file \"" << outFileName << "\"." << std::endl;

            return 1;
        }

        if ((findLenvOccurrences(hexDataOutFile) + findLenvOccurrences(hexDataOutFile.substr(findLenvOccurrences(hexDataOutFile) + 2)) + 2) == -1) {
            std::cerr << "DMI block 2 not found in file \"" << outFileName << "\"." << std::endl;

            return 1;
        }

        std::string dmiBlockOutFile = hexDataOutFile.substr(findLenvOccurrences(hexDataOutFile), ((findLenvOccurrences(hexDataOutFile) + findLenvOccurrences(hexDataOutFile.substr(findLenvOccurrences(hexDataOutFile) + 2)) + 2) - findLenvOccurrences(hexDataOutFile)) * 2);

        if (dmiBlock.length() != dmiBlockOutFile.length()) {
            std::cerr << "DMI blocks don't match size." << std::endl;

            return 1;
        }

        if (dmiBlock == dmiBlockOutFile) {
            std::cerr << "DMI blocks are identical." << std::endl;

            return 1;
        }

        hexDataOutFile.replace(findLenvOccurrences(hexDataOutFile), dmiBlock.length(), dmiBlock);
        writeHexDataToFile(outFileName, hexDataOutFile);

        std::cout << "Success: DMI successfully transfered from \"" << fileName << "\" to \"" << outFileName << "\"." << std::endl;
    }

    else if (strcmp(argv[2], "-info") == 0) {
        if (fileName == outFileName) {
            std::cout << "File name: " << fileName.substr(fileName.find_last_of("\\/") + 1) << std::endl
                << "File size: " << fileSize << " KB" << std::endl
                << "LENV offset 1: 0x" << std::hex << (lenvOffset1 / 2) << std::endl
                << "LENV offset 2: 0x" << (lenvOffset2 / 2) << std::dec << std::endl
                << "Encryption/decryption byte: 0x" << key << std::endl
                << "DMI region size: 0x" << std::hex << (dmiBlock.length() / 2) << std::dec << std::endl
                << "Data strings LENV block 1: " << amountDataLenvRegion1 << std::endl
                << "Data strings LENV block 2: " << amountDataLenvRegion2 << std::endl;
        }
        else {
            std::ofstream outFile(outFileName);

            if (outFile.is_open()) {
                outFile << "File name: " << fileName.substr(fileName.find_last_of("\\/") + 1) << std::endl
                    << "File size: " << fileSize << " KB" << std::endl
                    << "LENV offset 1: 0x" << std::hex << (lenvOffset1 / 2) << std::endl
                    << "LENV offset 2: 0x" << (lenvOffset2 / 2) << std::dec << std::endl
                    << "Encryption/decryption byte: 0x" << key << std::endl
                    << "DMI region size: 0x" << std::hex << (dmiBlock.length() / 2) << std::dec << std::endl
                    << "Data strings LENV block 1: " << amountDataLenvRegion1 << std::endl
                    << "Data strings LENV block 2: " << amountDataLenvRegion2 << std::endl;

                outFile.close();

                std::cout << "Success: Data has been saved to \"" << outFileName << "\"." << std::endl;
            }
            else {
                std::cerr << "Unable to save data to \"" << outFileName << "\"." << std::endl;

                return 1;
            }
        }
    }
    else {
        std::cerr << "Version:\t1.3.1" << std::endl
            << "Usage:\t\t" << programName << " \"Input file\" <-encrypt, -decrypt, -extract, -info> [\"Output file\"]" << std::endl
            << "\t\t" << programName << " \"DMI source file\" -transfer \"Target file\"" << std::endl;

        return 1;
    }

    return 0;
}