cmake_minimum_required(VERSION 3.10)
project(BankingSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Try to find mongocxx
find_package(mongocxx QUIET)
find_package(bsoncxx QUIET)

if(NOT mongocxx_FOUND)
    message(STATUS "mongocxx not found via CMake, trying pkg-config...")
    pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
    pkg_check_modules(BSONCXX REQUIRED libbsoncxx)
endif()

# Find Crow
find_path(CROW_INCLUDE_DIRS crow.h PATH_SUFFIXES crow)
find_library(CROW_LIBRARIES NAMES crow)

if(NOT CROW_INCLUDE_DIRS OR NOT CROW_LIBRARIES)
    message(STATUS "Crow not found via find_library, trying pkg-config...")
    pkg_check_modules(CROW crow)
endif()

# Include directories
include_directories(include)
if(CROW_INCLUDE_DIRS)
    include_directories(${CROW_INCLUDE_DIRS})
endif()

# Create executable
add_executable(banking_server
    src/main.cpp
    src/database.cpp
    src/auth.cpp
    src/routes.cpp
    src/utils.cpp
)

# Link libraries
target_link_libraries(banking_server 
    ${CROW_LIBRARIES}
    crypto
    ssl
    pthread
)

if(mongocxx_FOUND)
    target_link_libraries(banking_server mongo::mongocxx_shared mongo::bsoncxx_shared)
else()
    target_link_libraries(banking_server ${MONGOCXX_LIBRARIES} ${BSONCXX_LIBRARIES})
    target_include_directories(banking_server PRIVATE ${MONGOCXX_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS})
endif()

# Compiler flags
target_compile_options(banking_server PRIVATE ${MONGOCXX_CFLAGS_OTHER} ${BSONCXX_CFLAGS_OTHER})
