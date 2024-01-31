cmake_minimum_required(VERSION 3.27)
project(nauka)

set(CMAKE_CXX_STANDARD 23)

add_executable(nauka main.cpp)

set(MySQL_DIR "C:/libraries/mysql-con-cpp")
target_link_directories(nauka PRIVATE "C:/libraries/mysql-con-cpp/lib64/vs14")
target_include_directories(nauka PRIVATE "C:/libraries/mysql-con-cpp/include")
target_link_libraries(nauka "libcrypto.lib" "libssl.lib" "mysqlcppconn.lib")


