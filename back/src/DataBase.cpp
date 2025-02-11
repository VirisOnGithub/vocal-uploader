#include "DataBase.h"

namespace vc {

DataBase::DataBase(const std::string& url, const std::string& username, const std::string& password, const std::string& database) {
	m_Connection = std::unique_ptr<sql::Connection>(get_driver_instance()->connect(url, username, password));
	m_Connection->setSchema(database);
}

}  // namespace vc
