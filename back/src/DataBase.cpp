#include "DataBase.h"

#include <cppconn/prepared_statement.h>
#include <random>

namespace vc {

DataBase::DataBase(const std::string& url, const std::string& username, const std::string& password, const std::string& database) {
	m_Connection = std::unique_ptr<sql::Connection>(get_driver_instance()->connect(url, username, password));
	m_Connection->setSchema(database);
	PrepareStatements();
}

DataBase::~DataBase() {
	m_Connection->close();
}

void DataBase::PrepareStatements() {
	m_InsertGroup = std::unique_ptr<sql::PreparedStatement>(m_Connection->prepareStatement(
		"INSERT INTO Groupe(`UId`, `AccessToken`, `CreationDate`, `ExpirationDate`) VALUES (?, UUID(), NOW(), ?) RETURNING *;"));
	m_DeleteGroup = std::unique_ptr<sql::PreparedStatement>(m_Connection->prepareStatement("DELETE FROM Groupe WHERE UId = ?;"));
	m_InsertFile = std::unique_ptr<sql::PreparedStatement>(m_Connection->prepareStatement(
		"INSERT INTO File(`UId`, `Name`, `CreationDate`, `GroupId`) VALUES (?, ?, NOW(), ?) RETURNING *;"));
	m_DeleteFile = std::unique_ptr<sql::PreparedStatement>(m_Connection->prepareStatement("DELETE FROM File WHERE UId = ?;"));
	m_SelectFile = std::unique_ptr<sql::PreparedStatement>(m_Connection->prepareStatement("SELECT * FROM File WHERE UId = ?;"));
	m_SelectGroupFiles =
		std::unique_ptr<sql::PreparedStatement>(m_Connection->prepareStatement("SELECT * FROM File WHERE GroupId = ?;"));
}

Group DataBase::CreateGroup(int days_duration) {
	std::tm* expiration = GetTime();
	expiration->tm_mday += days_duration;
	std::mktime(expiration);

	m_InsertGroup->setString(1, GetNewID());				  // uid
	m_InsertGroup->setDateTime(2, TimeToString(expiration));  // expiration date

	// TODO: process duplicated key
	auto result = std::unique_ptr<sql::ResultSet>(m_InsertGroup->executeQuery());
	result->next();
	return {result->getString("UId"), result->getString("AccessToken")};
}

void DataBase::DeleteGroup(const std::string& groupUId) {
	// TODO: remove files before
	m_DeleteGroup->setString(1, groupUId);
	if (!m_DeleteGroup->execute()) {
		// error
	}
}


void DataBase::DeleteFile(const std::string& fileUId) {
	m_DeleteFile->setString(1, fileUId);
	if (!m_DeleteFile->execute()) {
		// error
	}
}


std::string DataBase::GetNewID() {
	static const std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";
	static std::string newId = "AAAAAAAAAAA";
	static std::random_device randomDevice;
	static std::mt19937 generator(randomDevice());
	static std::uniform_int_distribution<int> distrib(0, characters.size());

	for (int i = 0; i < 11; i++) {
		int nextIndex = distrib(generator);
		newId[i] = characters[nextIndex];
	}

	return newId;
}

std::tm* DataBase::GetTime() {
	time_t t = std::time(nullptr);
	std::tm* now = std::localtime(&t);
	return now;
}

std::string DataBase::TimeToString(std::tm* time) {
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d", time);
	return buffer;
}

File DataBase::CreateFile(const std::string& fileName, const std::string& groupId) {
	m_InsertFile->setString(1, GetNewID());
	m_InsertFile->setString(2, fileName);
	if (groupId.empty()) {
		m_InsertFile->setNull(3, sql::DataType::VARCHAR);
	} else {
		m_InsertFile->setString(3, groupId);
	}
	auto result = std::unique_ptr<sql::ResultSet>(m_InsertFile->executeQuery());
	result->next();
	return {result->getString("UId"), result->getString("Name"), result->getString("GroupId")};
}

std::vector<File> DataBase::GetGroupFiles(const std::string& groupUId) {
	m_SelectGroupFiles->setString(1, groupUId);
	auto result = std::unique_ptr<sql::ResultSet>(m_SelectFile->executeQuery());
	std::vector<File> files;
	while (result->next()) {
		files.push_back({result->getString("UId"), result->getString("Name"), result->getString("GroupId")});
	}
	return files;
}

std::optional<File> DataBase::GetFile(const std::string& fileUId) {
	m_SelectFile->setString(1, fileUId);
	auto result = std::unique_ptr<sql::ResultSet>(m_SelectFile->executeQuery());
	if (!result->next())
		return {};
	return std::make_optional<File>({result->getString("UId"), result->getString("Name"), result->getString("GroupId")});
}

}  // namespace vc
