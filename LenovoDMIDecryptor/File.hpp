#pragma once

#include "LenovoDMIDecryptor.hpp"

namespace File
{
	typedef struct _FILE_INFO
	{
		std::string FilePath;
		std::string FileName;
		uint64_t Size;
		std::string Created;
		std::string Modified;
		std::string Accessed;
		uint8_t* Buffer;
		bool Loaded = false;
		bool Dirty = false;
	} FILE_INFO, *PFILE_INFO;

	auto SelectFile(std::string& FilePath) -> bool;
	auto OpenFile(std::string& FilePath, FILE_INFO& FileInfo) -> bool;
	auto SaveFile(FILE_INFO& FileInfo) -> bool;
}