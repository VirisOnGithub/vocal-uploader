#include "FileSystem.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stx/stx-execpipe.h>

namespace fs = std::filesystem;

namespace vc {

bool FileSystem::FileExists(const std::string& a_FileName) {
	fs::path directory{m_Directory};
	return fs::exists(directory / a_FileName);
}

void FileSystem::DeleteFile(const std::string& a_FileName) {
	fs::path directory{m_Directory};
	fs::remove(directory / a_FileName);
}

std::optional<std::string> FileSystem::CompressAudioData(const std::string& a_Data) {
	stx::ExecPipe ep;

	ep.set_input_string(&a_Data);

	std::vector<std::string> args = {"/usr/bin/ffmpeg", "-hide_banner", "-loglevel", "panic", "-i", "pipe:0", "-b:a", "32k", "-map",
		"a", "-c:a", "libopus", "-f", "ogg", "-y", "pipe:1"};
	ep.add_exec(&args);

	std::string output;
	ep.set_output_string(&output);

	try {
		if (ep.run().all_return_codes_zero()) {
			return std::make_optional<std::string>(output);
		}
	} catch (std::runtime_error& e) {
		std::cout << "Error running children: " << e.what() << std::endl;
	}
	return {};
}

bool FileSystem::WriteFile(const std::string& a_FileName, const std::string& a_Data) {
	fs::path directory{m_Directory};
	std::ofstream file{directory / a_FileName};
	file << a_Data;
	return true;
}

}  // namespace vc
