#pragma once

#include <string>
#include <ctime>
#include <cppconn/driver.h>
#include <optional>
#include <vector>

namespace vc {

struct Group {
	std::string m_GroupUId;
	std::string m_GroupToken;
};

struct File {
	std::string m_FileUId;
	std::string m_FileName;
	std::string m_GroupUId;
};

class DataBase {
  public:
	DataBase(const std::string& url, const std::string& username, const std::string& password, const std::string& database);
	~DataBase();

	Group CreateGroup(int days_duration);
	File CreateFile(const std::string& fileName, const std::string& groupId = "");
	
	void DeleteGroup(const std::string& groupUId);
	void DeleteFile(const std::string& fileUId);

	std::vector<File> GetGroupFiles(const std::string& groupUId);
	std::optional<File> GetFile(const std::string& fileUId);

  private:
	void PrepareStatements();
	std::tm* GetTime();
	std::string TimeToString(std::tm* time);

	std::string GetNewID();

	std::unique_ptr<sql::PreparedStatement> m_InsertGroup;
	std::unique_ptr<sql::PreparedStatement> m_DeleteGroup;
	std::unique_ptr<sql::PreparedStatement> m_InsertFile;
	std::unique_ptr<sql::PreparedStatement> m_DeleteFile;
	std::unique_ptr<sql::PreparedStatement> m_SelectFile;
	std::unique_ptr<sql::PreparedStatement> m_SelectGroupFiles;

	std::unique_ptr<sql::Connection> m_Connection;
};
}  // namespace vc
