#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <Windows.h>

std::string readHexData(std::string fileName);
int findLenvOccurrences(std::string hexString);
std::string xorEncryptDecrypt(std::string lenvRegion, std::string key);
std::vector<uint8_t> lenvRegionToBytesArray(std::string hexString);
std::string getChecksum16(std::vector<uint8_t> bytesArray);
void writeHexDataToFile(std::string fileName, std::string hexData);