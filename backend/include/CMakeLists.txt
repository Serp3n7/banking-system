cmake_minimum_required(VERSION 3.10)
project(BankingSystem)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

pkg_check_modules(CROW REQUIRED crow)

include_directories(include)

add_executable(banking_server
    src/main.cpp
    src/database.cpp
    src/auth.cpp
    src/routes.cpp
    src/utils.cpp
)

target_link_libraries(banking_server 
    ${CROW_LIBRARIES}
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
    crypto
    ssl
)

target_include_directories(banking_server PRIVATE ${CROW_INCLUDE_DIRS})
