#include "Header.h"

std::string readHexData(std::string fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        return "";
    }

    std::ostringstream hexStringStream;
    hexStringStream << std::hex << std::setfill('0');
    char buffer;

    while (file.get(buffer)) {
        hexStringStream << std::setw(2) << static_cast<int>(static_cast<unsigned char>(buffer));
    }

    file.close();

    return hexStringStream.str();
}


int findLenvOccurrences(std::string hexString) {
	size_t pos = hexString.find("4c454e56");

	if (pos != std::string::npos) {
		return pos;
	}

	return 0;
}

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

std::vector<uint8_t> lenvRegionToBytesArray(std::string hexString) {
    std::vector<uint8_t> bytesArray;

    for (size_t i = 0; i < hexString.length(); i += 2) {
        std::string byteString = hexString.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
        bytesArray.push_back(byte);
    }

    return bytesArray;
}

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

void writeHexDataToFile(std::string fileName, std::string hexData) {
    std::ofstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        exit(1);
    }

    for (size_t i = 0; i < hexData.length(); i += 2) {
        std::string byteString = hexData.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
        file.put(byte);
    }

    file.close();

    return;
}