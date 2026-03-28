#include "File.hpp"

auto File::SelectFile(std::string& FilePath) -> bool
{
	OPENFILENAMEA Ofn = {};
	char Buffer[MAX_PATH] = {};
	Ofn.lStructSize = sizeof(Ofn);
	Ofn.hwndOwner = nullptr;
	Ofn.lpstrFilter = "All Files\0*.*\0";
	Ofn.lpstrFile = Buffer;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	auto Result = GetOpenFileNameA(&Ofn);

	if (Result)
	{
		FilePath = Buffer;
	}

	return Result != 0;
}

static auto GetFileNameFromPath(const std::string& FilePath) -> std::string
{
	auto LastSlash = FilePath.find_last_of("\\/");

	if (LastSlash == std::string::npos)
	{
		return FilePath;
	}

	return FilePath.substr(LastSlash + 1);
}

static auto FileTimeToString(const FILETIME& Ft) -> std::string
{
	FILETIME LocalFileTime{};
	SYSTEMTIME SystemTime{};

	if (!FileTimeToLocalFileTime(&Ft, &LocalFileTime))
	{
		return "";
	}

	if (!FileTimeToSystemTime(&LocalFileTime, &SystemTime))
	{
		return "";
	}

	char Buffer[64]{};

	sprintf_s(
		Buffer,
		"%04hu-%02hu-%02hu %02hu:%02hu:%02hu",
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond
	);

	return Buffer;
}

auto File::OpenFile(std::string& FilePath, FILE_INFO& FileInfo) -> bool
{
	if (FileInfo.Buffer)
	{
		delete[] FileInfo.Buffer;
		FileInfo.Buffer = nullptr;
	}

	FileInfo = {};

	if (FilePath.empty())
	{
		return false;
	}

	auto FileHandle = CreateFileA(FilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	LARGE_INTEGER FileSize{};

	if (!GetFileSizeEx(FileHandle, &FileSize))
	{
		CloseHandle(FileHandle);
		return false;
	}

	if (FileSize.QuadPart <= 0)
	{
		CloseHandle(FileHandle);
		return false;
	}

	if (FileSize.QuadPart > 0xFFFFFFFF)
	{
		CloseHandle(FileHandle);
		return false;
	}

	FILETIME CreationTime{}, AccessTime{}, WriteTime{};

	if (!GetFileTime(FileHandle, &CreationTime, &AccessTime, &WriteTime))
	{
		CloseHandle(FileHandle);
		return false;
	}

	auto FileBuffer = new uint8_t[FileSize.QuadPart];
	DWORD BytesRead = 0;

	if (!ReadFile(FileHandle, FileBuffer, FileSize.QuadPart, &BytesRead, nullptr))
	{
		delete[] FileBuffer;
		CloseHandle(FileHandle);
		return false;
	}

	if (BytesRead != FileSize.QuadPart)
	{
		delete[] FileBuffer;
		CloseHandle(FileHandle);
		return false;
	}

	CloseHandle(FileHandle);

	FileInfo.FilePath = FilePath;
	FileInfo.FileName = GetFileNameFromPath(FilePath);
	FileInfo.Size = FileSize.QuadPart;
	FileInfo.Created = FileTimeToString(CreationTime);
	FileInfo.Modified = FileTimeToString(WriteTime);
	FileInfo.Accessed = FileTimeToString(AccessTime);
	FileInfo.Buffer = FileBuffer;
	FileInfo.Loaded = true;

	return true;
}

auto File::SaveFile(FILE_INFO& FileInfo) -> bool
{
	if (!FileInfo.Loaded || !FileInfo.Buffer || FileInfo.FilePath.empty())
	{
		return false;
	}

	auto FileHandle = CreateFileA(FileInfo.FilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD BytesWritten = 0;

	if (!WriteFile(FileHandle, FileInfo.Buffer, static_cast<DWORD>(FileInfo.Size), &BytesWritten, nullptr))
	{
		CloseHandle(FileHandle);
		return false;
	}

	CloseHandle(FileHandle);

	if (BytesWritten != FileInfo.Size)
	{
		return false;
	}

	FileInfo.Dirty = false;

	return true;
}