#pragma once

#include <string>

#include <cppconn/driver.h>

namespace vc {
class DataBase {
  public:
	DataBase(const std::string& url, const std::string& username, const std::string& password, const std::string& database);

  private:
	std::unique_ptr<sql::Connection> m_Connection;
};
}  // namespace vc
