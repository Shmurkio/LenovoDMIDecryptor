#include "Header.h"

std::string readFileAsHexString(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        return "";
    }

    /* Get the file size for progress calculation */
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::ostringstream hexStringStream;
    hexStringStream << std::hex << std::setfill('0');
    char buffer;

    /* Progress variables */
    const int bufferSize = 2048;
    double bytesRead = 0;
    double progress = 0;

    while (file.get(buffer)) {
        hexStringStream << std::setw(2) << static_cast<int>(static_cast<unsigned char>(buffer));

        /* Check progress */
        bytesRead++;
        double newProgress = (bytesRead / fileSize) * 100;

        /* Display progress if it has changed significantly */
        if (std::abs(newProgress - progress) >= 1.0) {
            progress = newProgress;
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cout << "Reading dump: " << static_cast<int>(progress) << "%\r";
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
            std::cout << static_cast<int>(progress) << "%\r";
            std::cout.flush();
        }
    }

    file.close();
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Reading dump: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
    std::cout << "100%\n";

    return hexStringStream.str();
}


std::uintmax_t getFileSize(const std::string& filename) {
    HANDLE file = CreateFileA(
        filename.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (file == INVALID_HANDLE_VALUE) {
        return 0; /* Close if an error while opening the file occurs */
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size)) {
        CloseHandle(file);
        return 0; /* Close if an error while getting file size occurs */
    }

    CloseHandle(file);

    return static_cast<std::uintmax_t>(size.QuadPart);
}

int getLenvPos1(std::string hexString) {
    std::string searchString = "4c454e56"; /* Search string is "LENV" */
    size_t pos1 = hexString.find(searchString); /* Searches for "LENV" */

    if (pos1 != std::string::npos) { /* If found */
        int lenvPos1 = static_cast<int>(pos1);

        return lenvPos1;
    }
    else { /* If not found */
        return 1;
    }
}

int getLenvPos2(std::string hexString) {
    std::string searchString = "4c454e56"; /* Searche string is "LENV" */
    size_t pos1 = hexString.find(searchString);
    size_t pos2 = hexString.find(searchString, pos1 + 1); /* Searches for "LENV" */

    if (pos2 != std::string::npos) { /* If found */
        int lenvPos2 = static_cast<int>(pos2);

        return lenvPos2;
    }
    else { /* If not found */
        return 1;
    }
}

std::string xorEncryptDecrypt(const std::string& encryptedData, const std::string& key) {
    std::stringstream result;
    for (size_t i = 0; i < encryptedData.size(); i += 2) { /* Iterate over the HEX string */
        int byte = std::stoi(encryptedData.substr(i, 2), nullptr, 16);
        int keyByte = std::stoi(key.substr(i % key.size(), 2), nullptr, 16);
        int decryptedByte = byte ^ keyByte; /* Perform XOR operation */
        result << std::hex << std::setw(2) << std::setfill('0') << decryptedByte; /* Convert result back to HEX and append it */
    }
    return result.str();
}

void writeHexStringToFile(const std::string& filename, const std::string& hexString) {
    std::ofstream outFile(filename, std::ios::binary); /* Open output file in binary mode */

    for (size_t i = 0; i < hexString.length(); i += 2) { /* Iterate over the HEX string */
        std::string byteString = hexString.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
        outFile.put(byte); /* Write to output file */
    }

    outFile.close();
}

int main(int argc, char** argv) {
    SetConsoleTitleA("Lenovo DMI Decryption Tool | v1.1");
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << "Lenovo DMI Decryption Tool by Shmurkio (Discord: .negroni_)\n\n";
    if (argc < 2) { /* Close if no file is selected */
        return 1;
    }
    std::string filename = argv[1]; /* File name and path of the BIOS dump */
    std::uintmax_t filesize = getFileSize(filename);
    if (filesize != 16777216) {
        return 1; /* Close if dump is not a full 16 mb dump */
    }
    std::string hexString = readFileAsHexString(filename);
    int lenvPos1 = getLenvPos1(hexString); /* Gets position of the first LENV block in the HEX data */
    int lenvPos2 = getLenvPos2(hexString); /* Gets position of the second LENV block in the HEX data */
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Offset LENV1: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
    std::cout << "0x" << (lenvPos1 / 2) << std::endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Offset LENV2: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
    std::cout << "0x" << (lenvPos2 / 2) << std::endl;
    int keyPos = lenvPos1 - 2; /* Gets position of the decryption key, 2 bits before first LENV block */
    std::string decryptionKey = hexString.substr(keyPos, 2); /* Gets value of the decryption key */
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Encryption key: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
    std::cout << decryptionKey << std::endl << std::endl;
    std::string firstDmiBlock = hexString.substr((lenvPos1 + 8), 8184); /* Gets first LENV block */
    std::string secondDmiBlock = hexString.substr((lenvPos2 + 8), 8184); /* Gets second LENV block */
    std::string firstDmiBlockResult = xorEncryptDecrypt(firstDmiBlock, decryptionKey); /* Decrypts/encrypts first LENV block */
    std::string secondDmiBlockResult = xorEncryptDecrypt(secondDmiBlock, decryptionKey); /* Decrypts/encrypts first LENV block */
    hexString.replace((lenvPos1 + 8), firstDmiBlock.length(), firstDmiBlockResult); /* Replaces first DMI block with decrypted/encrypted DMI block*/
    hexString.replace((lenvPos2 + 8), secondDmiBlock.length(), secondDmiBlockResult); /* Replaces second DMI block with decrypted/encrypted DMI block*/
    writeHexStringToFile(filename, hexString); /* Writes the data to the BIOS dump */
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Successfully encrypted/decrypted both DMI blocks, hit enter to exit";
    system("PAUSE >NUL 2>NUL");

    return 0;
}