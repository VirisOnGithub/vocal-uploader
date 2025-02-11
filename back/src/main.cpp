#include <iostream>

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

/**
 * MySQL_Connection(Driver * _driver,
          ::sql::mysql::NativeAPI::NativeConnectionWrapper & _proxy,
          const sql::SQLString& hostName,
          const sql::SQLString& userName,
          const sql::SQLString& password);
 */

int main(int argc, char **argv)
{
    sql::mysql::MySQL_Driver driver;
    sql::Connection *connection = driver.connect("tcp://127.0.0.1:3306", "root", "super_strong_password");
    connection->setSchema("vcupload");

    sql::PreparedStatement* statement = connection->prepareStatement("INSERT INTO test(id) VALUES (?)");
    auto res = statement->executeQuery();
    try
    {
        while (res->next())
        {
            std::cout << "\t... MySQL replies: ";
            /* Access column data by alias or column name */
            std::cout << res->getString("_message") << std::endl;
            std::cout << "\t... MySQL says it again: ";
            /* Access column data by numeric offset, 1 is the first column */
            std::cout << res->getString(1) << std::endl;
        }
        delete res;
        delete statement;
        delete connection;
    }
    catch (sql::SQLException &e)
    {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "
                  << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    return 0;
}
