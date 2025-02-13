#pragma once

#include <cstdint>
#include "FileSystem.h"
#include "DataBase.h"

namespace vc {
class WebServer {
  public:
	WebServer(std::uint16_t port, FileSystem& a_FileSystem, DataBase& a_DataBase);

  private:
	FileSystem& m_FileSystem;
	DataBase& m_DataBase;
};
}  // namespace vc
