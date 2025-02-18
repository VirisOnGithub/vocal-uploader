#pragma once

#include <string>
#include <optional>

namespace vc {
class FileSystem {
  public:
	FileSystem(std::string&& a_Directory) : m_Directory(std::move(a_Directory)) {}

	bool FileExists(const std::string& a_FileName);
	void DeleteFile(const std::string& a_FileName);
	bool WriteFile(const std::string& a_FileName, const std::string& a_Data);
	void ReadFile(const std::string& a_FileName);

	static std::optional<std::string> CompressAudioData(const std::string& a_Data);

	const std::string& GetDirectory() {
		return m_Directory;
	}

  private:
	std::string m_Directory;
};
}  // namespace vc
