#include "FileSystem.h"

#include <filesystem>

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

}  // namespace vc
