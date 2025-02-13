#include "FileSystem.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace vc {

bool FileSystem::FileExists(const std::string& a_FileName) {
	fs::path directory {m_Directory};
	return fs::exists(directory / a_FileName);
}

void FileSystem::DeleteFile(const std::string& a_FileName) {
	fs::path directory {m_Directory};
	fs::remove(directory / a_FileName);
}

void FileSystem::WriteFile(const std::string& a_FileName, const std::string& a_Data) {
	fs::path directory {m_Directory};
	std::ofstream file {directory / a_FileName};
	file << a_Data;
}

}  // namespace vc
