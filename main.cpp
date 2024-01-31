#include <iostream>
#include <string>
#include <map>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>

void printInitialMessage() {
    std::cout << "========== TestApplicationV23 ==========" << std::endl;
    std::cout << "[1] - Login" << std::endl;
    std::cout << "[2] - Register" << std::endl;
    std::cout << "========== TestApplicationV23 ==========" << std::endl;
}

void printAccountInformation(sql::Connection *conn, std::string &login) {
    sql::PreparedStatement *pstmt;
    sql::ResultSet *result;

    pstmt = conn->prepareStatement("SELECT userName, level, experience, gold FROM accounts WHERE login=?");
    pstmt->setString(1, login);
    result = pstmt->executeQuery();

    std::string userName = result->getString("userName");
    int level = result->getInt("level");
    int experience = result->getInt("experience");
    int gold = result->getInt("gold");

    std::cout << "=============== ACCOUNT INFORMATION ===============" << std::endl;
    std::cout << "UserName: " << userName << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Experience: " << experience << std::endl;
    std::cout << "Gold: " << gold << std::endl;
    std::cout << "=============== ACCOUNT INFORMATION ===============" << std::endl;
}

sql::Connection* connectToDatabase(const std::string &hostDB, const std::string &userDB, const std::string &passwordDB, const std::string &database) {
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    sql::Connection *conn = driver->connect("tcp://" + hostDB + ":3306", userDB, passwordDB);
    conn->setSchema(database);
    return conn;
}

void closeDatabaseConnection(sql::Connection *conn) {
    delete conn;
}

bool isLoginExisting(sql::Connection *conn, std::string const &login) {
    sql::PreparedStatement *existenceCheckStatement = conn->prepareStatement("SELECT COUNT(*) FROM accounts WHERE login = ?");
    existenceCheckStatement->setString(1, login);

    sql::ResultSet *result = existenceCheckStatement->executeQuery();
    result->next();
    int count = result->getInt(1);

    delete existenceCheckStatement;
    delete result;

    return count > 0;
}

bool isUserNameExisting(sql::Connection *conn, std::string const &userName) {
    sql::PreparedStatement *existenceCheckStatement = conn->prepareStatement("SELECT COUNT(*) FROM accounts WHERE userName = ?");
    existenceCheckStatement->setString(1, userName);

    sql::ResultSet *result = existenceCheckStatement->executeQuery();
    result->next();
    int count = result->getInt(1);

    delete existenceCheckStatement;
    delete result;

    return count > 0;
}

bool isCorrectData(sql::Connection *conn, std::string &login, std::string &password) {
    try {
        sql::PreparedStatement *pstmt;
        sql::ResultSet *result;

        pstmt = conn->prepareStatement("SELECT * FROM accounts WHERE login=?");
        pstmt->setString(1, login);

        result = pstmt->executeQuery();

        if(result->next()) {
            std::string correctPassword = result->getString("password");

            if(password == correctPassword) {
                delete result;
                delete pstmt;
                return true;
            }
        }

        delete result;
        delete pstmt;
        return false;
    } catch(sql::SQLException &e) {
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
}

void addUserToDatabase(sql::Connection *conn, std::string const &login, std::string const &userName, std::string const &password) {
    sql::PreparedStatement *addStatement = conn->prepareStatement("INSERT INTO accounts (login, userName, password) VALUES (?, ?, ?)");

    addStatement->setString(1, login);
    addStatement->setString(2, userName);
    addStatement->setString(3, password);

    addStatement->executeUpdate();

    delete addStatement;
}

int main() {
    std::string const hostDB = "localhost";
    std::string const userDB = "root";
    std::string const passwordDB = "test123!";
    std::string const database = "exampleDB";

    sql::Connection *conn = connectToDatabase(hostDB, userDB, passwordDB, database);

    printInitialMessage();

    int userChoice;

    std::cout << "Select an option [1-2]: ";
    std::cin >> userChoice;

    switch(userChoice) {
        case 1: {
            std::string login;
            std::string password;

            while(true) {
                std::cout << "Enter login: ";
                std::cin >> login;

                std::cout << "Enter password: ";
                std::cin >> password;

                if(isCorrectData(conn, login, password)) {
                    printAccountInformation(conn, login);
                    break;
                } else {
                    std::cout << "Invalid login or password!" << std::endl << std::endl;
                    continue;
                }
            }

        }
            break;


        case 2: {
            std::string login;
            std::string userName;
            std::string password;
            std::string confirmedPassword;

            while(true) {
                std::cout << "Enter login: ";
                std::cin >> login;

                std::cout << "Enter userName: ";
                std::cin >> userName;

                std::cout << "Enter password: ";
                std::cin >> password;

                std::cout << "Confirm password: ";
                std::cin >> confirmedPassword;

                if(isLoginExisting(conn, login)) {
                    std::cout << "Login is already in use!" << std::endl << std::endl;
                    continue;
                }

                if(isUserNameExisting(conn, userName)) {
                    std::cout << "Username is already in use!" << std::endl << std::endl;
                    continue;
                }

                if(password != confirmedPassword) {
                    std::cout << "The entered passwords do not match!" << std::endl << std::endl;
                    continue;
                }
                addUserToDatabase(conn, login, userName, password);
                std::cout << "You have successfully registered your account!" << std::endl << std::endl;
                break;
            }

        }
            break;

        default:
            std::cout << "Invalid Value!" << std::endl;
            break;
    }
}